#include <filesystem>
#include <fstream>
#include <sstream>
#include <gltf.h>

#include "animation/animation.h"
#include "animation/channel.h"
#include "core/engine.h"
#include "core/components/rendercomponent.h"
#include "core/components/transformcomponent.h"
#include "display/renderer.h"
#include "physics/collidercomponent.h"
#include "physics/physicscomponent.h"
#include "animation/components/animationcomponent.h"
#include "display/vertexdata.h"
#include "assets/gltfimporter.h"
#include "assets/importresult.h"
#include "animation/animation.h"
#include "json/jsonloader.h"
#include "assets/assetset.h"
#include <util/filemanager.h>

using namespace Boiler;
using namespace std;

GLTFImporter::GLTFImporter(Boiler::Engine &engine) : engine(engine), logger("GLTF Importer") { }

AssetId GLTFImporter::loadImage(const std::string &gltfPath, const gltf::Image &image, std::vector<std::pair<AssetId, ImageData>> &textureIds)
{
	assert(image.uri.length() > 0);

	std::filesystem::path imagePath = FileManager::getDirectory(gltfPath) / image.uri;
	logger.log("Loading image: {}", imagePath.string());
	ImageData imageData = ImageLoader::load(imagePath.string());

	// load the texture into GPU mem
	AssetId textureId = engine.getRenderer().loadTexture(imageData, imageData.hasAlpha ? TextureType::RGBA_SRGB : TextureType::RGB_SRGB);
	textureIds.push_back(std::make_pair(textureId, std::move(imageData)));

	return textureId;
}

std::shared_ptr<GLTFModel> GLTFImporter::import(AssetSet &assetSet, const std::string &gltfPath)
{
	std::vector<std::vector<std::byte>> buffers;
	ImportResult result;

	const std::string jsonString = JsonLoader::loadJson(gltfPath);

	logger.log("Importing: {}", gltfPath);
	gltf::Model model = gltf::load(gltfPath, jsonString);

	logger.log("Loading model: {}", gltfPath);

	filesystem::path filePath(gltfPath);
	filesystem::path basePath = filePath.parent_path();

	// load all of the buffers
	for (const auto &buffer : model.buffers)
	{
		buffers.push_back(loadBuffer(basePath.string(), buffer));
	}

	// load materials
	std::vector<std::pair<AssetId, ImageData>> textureIds;
	textureIds.reserve(model.images.size());
	for (const gltf::Image &image : model.images)
	{
		loadImage(gltfPath, image, textureIds);
	}

	for (size_t i = 0; i < model.materials.size(); ++i)
	{
		Material newMaterial;
		const gltf::Material &material = model.materials[i];
		if (material.pbrMetallicRoughness.has_value())
		{
			// albedo texture
			if (material.pbrMetallicRoughness.value().baseColorTexture.has_value())
			{
				const gltf::MaterialTexture &matTexture = material.pbrMetallicRoughness.value().baseColorTexture.value();
				const gltf::Texture &texture = model.textures[matTexture.index.value()];
				newMaterial.albedoTexture = textureIds[texture.source.value()].first;
			}

			// metallic roughness texture
			if (material.pbrMetallicRoughness.value().metallicRoughnessTexture.has_value())
			{
				const gltf::MaterialTexture &matTexture = material.pbrMetallicRoughness.value().metallicRoughnessTexture.value();
				const gltf::Texture &texture = model.textures[matTexture.index.value()];
				newMaterial.metallicRougnessTexture = textureIds[texture.source.value()].first;
			}

			// normal map texture
			if (material.normalTexture.has_value())
			{
				const gltf::MaterialTexture &matTexture = material.normalTexture.value();
				const gltf::Texture &texture = model.textures[matTexture.index.value()];
				newMaterial.normalTexture = textureIds[texture.source.value()].first;
			}

			newMaterial.diffuse = vec4(1, 1, 1, 1);
			if (material.pbrMetallicRoughness->baseColorFactor.has_value())
			{
				auto &colorFactor = material.pbrMetallicRoughness->baseColorFactor.value();
				newMaterial.diffuse = { colorFactor[0], colorFactor[1], colorFactor[2], colorFactor[3] };
			}
			if (material.alphaMode == "BLEND")
			{
				newMaterial.alphaMode = AlphaMode::blend;
			}
			else if (material.alphaMode == "MASK")
			{
				newMaterial.alphaMode = AlphaMode::mask;
			}
			else
			{
				newMaterial.alphaMode = AlphaMode::opaque;
			}
		}
		AssetId materialId = assetSet.materials.add(std::move(newMaterial));
		result.materialsIds.push_back(materialId);
	}

	// Model accessors which are used for typed access into buffers
	gltf::ModelAccessors modelAccess(model, buffers);

	unsigned int primitiveCount = 0;
	for (const auto &mesh : model.meshes)
	{
		logger.log("Loading mesh: {}", mesh.name);
		Mesh newMesh;
		newMesh.name = mesh.name;
		std::optional<vec3> meshMin, meshMax;

		for (int i = 0; i < mesh.primitives.size(); ++i)
		{
			auto &gltfPrimitive = mesh.primitives[i];
			logger.log("Loading primitive with position data index: {}", i);

			VertexData vertexData = getPrimitiveData(engine, modelAccess, gltfPrimitive);
			AssetId bufferId = engine.getRenderer().loadPrimitive(vertexData);

			// TODO: generate collision volumes
			const gltf::Accessor &positionAccessor = model.accessors.at(gltfPrimitive.attributes.find(gltf::attributes::POSITION)->second);
			vec3 min(positionAccessor.min[0].asFloat, positionAccessor.min[1].asFloat, positionAccessor.min[2].asFloat);
			vec3 max(positionAccessor.max[0].asFloat, positionAccessor.max[1].asFloat, positionAccessor.max[2].asFloat);
			AssetId primitiveId = assetSet.primitives.add(Primitive(bufferId, std::move(vertexData), min, max));

			// keep track of min/max for the entire mesh
			meshMin = !meshMin.has_value() ? min : glm::min(meshMin.value(), min);
			meshMax = !meshMax.has_value() ? max : glm::max(meshMax.value(), max);

			// setup material if any
			if (gltfPrimitive.material.has_value())
			{
				assetSet.primitives.get(primitiveId).materialId = result.materialsIds[gltfPrimitive.material.value()];
			}
			newMesh.primitives.push_back(primitiveId);
			newMesh.min = meshMin.value();
			newMesh.max = meshMax.value();
			primitiveCount++;
		}
		result.meshes.push_back(newMesh);
	}
	logger.log("Loaded {} total primitives across all meshes", primitiveCount);

	// load all animations
	Animator &animator = engine.getAnimator();
	for (const gltf::Animation &gltfAnim : model.animations)
	{
		Animation animation(gltfAnim.name);
		const auto loadSampler = [this, &model, &animator, &modelAccess](const gltf::Sampler &gltfSamp)
		{
			// load key frame times
			std::vector<float> keyFrameTimes;

			const auto &timeAccessor = modelAccess.getTypedAccessor<float, 1>(model.accessors[gltfSamp.input]);
			keyFrameTimes.reserve(timeAccessor.size());
			for (const float *values : timeAccessor)
			{
				keyFrameTimes.push_back(values[0]);
			}

			// load key frame values
			const gltf::Accessor &access = model.accessors[gltfSamp.output];
			const gltf::BufferView &buffView = model.bufferViews[access.bufferView.value()];

			assert(access.componentType == gltf::ComponentType::FLOAT);

			// copy animation data bytes
			assert(buffView.byteLength.has_value());
			const size_t dataSize = buffView.byteLength.value() - access.byteOffset;
			std::vector<std::byte> animData(dataSize);
			std::memcpy(animData.data(), modelAccess.getPointer(access), animData.size());

			return animator.addSampler(AnimationSampler(std::move(keyFrameTimes), std::move(animData)));
		};

		for (const gltf::Channel &gltfChan : gltfAnim.channels)
		{
			SamplerId samplerId = loadSampler(gltfAnim.samplers[gltfChan.sampler]);
			animation.addChannel(Channel(gltfChan.target.node.value(), gltfChan.target.path, samplerId));
		}
		result.animations.push_back(animator.addAnimation(std::move(animation)));
	}

	logger.log("Imported {} materials", result.materialsIds.size());
	logger.log("Imported {} meshes", result.meshes.size());
	logger.log("Imported {} animations", result.animations.size());

	return std::make_shared<GLTFModel>(gltfPath, std::move(buffers), std::move(model), std::move(result));
}

VertexData GLTFImporter::getPrimitiveData(Engine &engine, const gltf::ModelAccessors &modelAccess, const gltf::Primitive &primitive)
{
	if (primitive.mode.has_value())
	{
		// TODO: Add support for other modes
		assert(primitive.mode == 4);
	}
	using namespace gltf;

	// get the primitive's position data
	std::vector<Vertex> vertices;

	assert(modelAccess.getAccessor(primitive, attributes::POSITION).type == gltf::AccessorType::VEC3);
	auto positionAccess = modelAccess.getTypedAccessor<float, 3>(primitive, attributes::POSITION);
	for (auto values : positionAccess)
	{
		Vertex vertex({ values[0], values[1], values[2] });
		vertex.colour = { 1, 1, 1, 1 };
		vertices.push_back(vertex);
	}

	// vertex normals
	assert(primitive.attributes.find(attributes::NORMAL) != primitive.attributes.end());
	auto normalAccess = modelAccess.getTypedAccessor<float, 3>(primitive, attributes::NORMAL);
	unsigned int vertexIndex = 0;
	for (auto normal : normalAccess)
	{
		vertices[vertexIndex++].normal = { normal[0], normal[1], normal[2] };
	}

	// load the primitive indices
	std::vector<uint32_t> indices;
	assert(primitive.indices.has_value());
	if (primitive.indices.has_value())
	{
		assert(modelAccess.getModel().accessors[primitive.indices.value()].type == AccessorType::SCALAR);
		const auto &indexAccessor = modelAccess.getModel().accessors[primitive.indices.value()];
		if (indexAccessor.componentType == gltf::ComponentType::UNSIGNED_SHORT)
		{
			auto indexAccess = modelAccess.getTypedAccessor<unsigned short, 1>(primitive.indices.value());
			for (auto values : indexAccess)
			{
				indices.push_back(values[0]);
			}
		}
		else if (indexAccessor.componentType == gltf::ComponentType::UNSIGNED_INT)
		{
			auto indexAccess = modelAccess.getTypedAccessor<unsigned int, 1>(primitive.indices.value());
			for (auto values : indexAccess)
			{
				indices.push_back(values[0]);
			}
		}
	}
	else
	{
		// generate simple indices
		for (size_t i = 0; i < vertices.size(); ++i)
		{
			indices.push_back(i);
		}
	}

	// load texture coordinates
	const auto &texCoordAttr = primitive.attributes.find(attributes::TEXCOORD_0);
	if (texCoordAttr != primitive.attributes.end())
	{
		assert(modelAccess.getModel().accessors[texCoordAttr->second].type == gltf::AccessorType::VEC2);
		const auto &accessor = modelAccess.getModel().accessors[texCoordAttr->second];
		auto texCoordAccess = modelAccess.getTypedAccessor<float, 2>(accessor);

		long vertexIdx = 0;
		for (auto values : texCoordAccess)
		{
			vertices[vertexIdx++].textureCoordinates = { values[0], values[1] };
		}
	}

	return VertexData(vertices, indices);
}
