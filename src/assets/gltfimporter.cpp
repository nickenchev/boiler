#include <filesystem>
#include <fstream>
#include <sstream>

#include "animation/animation.h"
#include "animation/channel.h"
#include "core/engine.h"
#include "core/components/rendercomponent.h"
#include "core/components/transformcomponent.h"
#include "animation/components/animationcomponent.h"
#include "video/vertexdata.h"
#include "assets/gltfimporter.h"
#include "assets/importresult.h"

#include "animation/animation.h"

#include "typedaccessor.h"

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

using namespace Boiler;

GLTFImporter::GLTFImporter(Boiler::Engine &engine, const std::string &gltfPath) : logger("GLTF Importer"), engine(engine)
{
	std::ifstream infile(gltfPath);
	std::stringstream buffer;
	buffer << infile.rdbuf();
	const std::string jsonString = buffer.str();
	infile.close();

	logger.log("Importing: {}", gltfPath);
	model = gltf::load(jsonString);

	using namespace std;
	logger.log("Loading model: {}", gltfPath);

	filesystem::path filePath(gltfPath);
	filesystem::path basePath = filePath.parent_path();

	// load all of the buffers
	for (const auto &buffer : model.buffers)
	{
		buffers.push_back(loadBuffer(basePath.string(), buffer));
	}

	// load materials
	for (size_t i = 0; i < model.materials.size(); ++i)
	{
		Material &newMaterial = engine.getRenderer().createMaterial();
		const gltf::Material &material = model.materials[i];
		if (material.pbrMetallicRoughness.has_value())
		{
			if (material.pbrMetallicRoughness.value().baseColorTexture.has_value())
			{
				const gltf::MaterialTexture &matTexture = material.pbrMetallicRoughness.value().baseColorTexture.value();
				const gltf::Texture &texture = model.textures[matTexture.index.value()];
				const gltf::Image &image = model.images[texture.source.value()];

				assert(image.uri.length() > 0);
				filesystem::path imagePath = basePath;
				imagePath.append(image.uri);
				const ImageData imageData = ImageLoader::load(imagePath.string());

				// load the texture into GPU mem
				newMaterial.baseTexture = engine.getRenderer().loadTexture(imageData); 
			}

			newMaterial.diffuse = vec4(1, 1, 1, 1);
			if (material.pbrMetallicRoughness->baseColorFactor.has_value())
			{
				auto colorFactor = material.pbrMetallicRoughness->baseColorFactor.value();
				newMaterial.diffuse = {colorFactor[0], colorFactor[1], colorFactor[2], colorFactor[3]};
			}
			if (material.alphaMode == "BLEND")
			{
				newMaterial.alphaMode = AlphaMode::BLEND;
			}
			else if (material.alphaMode == "MASK")
			{
				newMaterial.alphaMode = AlphaMode::MASK;
			}
			else
			{
				newMaterial.alphaMode = AlphaMode::OPAQUE;
			}
		}
		result.materials.push_back(newMaterial);
	}

	// Model accessors which are used for typed access into buffers
	gltf::ModelAccessors modelAccess(model, buffers);

	for (const auto &mesh : model.meshes)
	{
		logger.log("Loading mesh: {}", mesh.name);
		Mesh newMesh;
		for (auto &gltfPrimitive : mesh.primitives)
		{
			Primitive meshPrimitive = loadPrimitive(engine, modelAccess, gltfPrimitive);
			// setup material if any
			if (gltfPrimitive.material.has_value())
			{
				const Material &material = result.materials[gltfPrimitive.material.value()];
				meshPrimitive.materialId = material.getAssetId();
			}
			newMesh.primitives.push_back(meshPrimitive);
		}
		result.meshes.push_back(newMesh);
	}

	// load all animations
    Animator &animator = engine.getAnimator();
	for (const gltf::Animation &gltfAnim : model.animations)
	{
		Animation animation(gltfAnim.name);
		const auto loadSampler = [this, &animator, &modelAccess](const gltf::Sampler &gltfSamp)
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

	logger.log("Imported {} materials", result.materials.size());
	logger.log("Imported {} meshes", result.meshes.size());
	logger.log("Imported {} animations", result.animations.size());
}

Primitive GLTFImporter::loadPrimitive(Engine &engine, const gltf::ModelAccessors &modelAccess, const gltf::Primitive &primitive)
{
	if (primitive.mode.has_value())
	{
		// TODO: Add support for other modes
		assert(primitive.mode == 4);
	}
	using namespace gltf::attributes;

	// get the primitive's position data
	std::vector<Vertex> vertices;
	auto positionAccess = modelAccess.getTypedAccessor<float, 3>(primitive, POSITION);
	for (auto values : positionAccess)
	{
		Vertex vertex({values[0], values[1], values[2]});
		vertex.colour = {1, 1, 1, 1};
		vertices.push_back(vertex);
	}

	assert(primitive.attributes.find(NORMAL) != primitive.attributes.end());

	auto normalAccess = modelAccess.getTypedAccessor<float, 3>(primitive, NORMAL);
	unsigned int vertexIndex = 0;
	for (auto normal : normalAccess)
	{
		vertices[vertexIndex++].normal = {normal[0], normal[1], normal[2]};
	}

	// load the primitive indices
	std::vector<uint32_t> indices;
	assert(primitive.indices.has_value());
	if (primitive.indices.has_value())
	{
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
	const auto &texCoordAttr = primitive.attributes.find(TEXCOORD_0);
	if (texCoordAttr != primitive.attributes.end())
	{
		const auto &accessor = modelAccess.getModel().accessors[texCoordAttr->second];
		auto texCoordAccess = modelAccess.getTypedAccessor<float, 2>(accessor);

		long vertexIdx = 0;
		for (auto values : texCoordAccess)
		{
			vertices[vertexIdx++].textureCoordinate = {values[0], values[1]};
		}
	}

	const VertexData vertData(vertices, indices);
	return engine.getRenderer().loadPrimitive(vertData);
}

Entity GLTFImporter::loadNode(std::vector<Entity> &nodeEntities, const Entity nodeEntity, int nodeIndex, const Entity parentEntity) const
{
	if (!nodeEntities[nodeIndex] != Entity::NO_ENTITY)
	{
		EntityComponentSystem &ecs = engine.getEcs();
		nodeEntities[nodeIndex] = nodeEntity;

		// set parent entity if provided
		if (parentEntity != Entity::NO_ENTITY)
		{
			ecs.createComponent<ParentComponent>(nodeEntity, parentEntity);
		}

		const gltf::Node &node = model.nodes[nodeIndex];
		logger.log("Processing node: {}", node.name);

		// assign mesh
		if (node.mesh.has_value())
		{
			auto render = ecs.createComponent<RenderComponent>(nodeEntity);
			render->mesh = result.meshes[node.mesh.value()]; // use mesh-id instead of copying
		}

		auto transform = ecs.createComponent<TransformComponent>(nodeEntity, Rect(0, 0, 0, 0));
		// decompose a matrix if available, otherwise try to load transformations directly
		if (node.matrix.has_value())
		{
			const auto &matrixArray = node.matrix.value();
			mat4 matrix = glm::make_mat4(matrixArray.data());
			vec3 scale, skew, position;
			glm::quat orientation;
			vec4 perspective;

			glm::decompose(matrix, scale, orientation, position, skew, perspective);
			transform->setPosition(position);
			transform->setScale(scale);
			transform->setOrientation(orientation);
		}
		else
		{
			// otherwise load transformation directly
			if (node.scale.has_value())
			{
				transform->setScale({
					node.scale.value()[0],
					node.scale.value()[1],
					node.scale.value()[2]
				});
			}
			if (node.translation.has_value())
			{
				transform->setPosition({
					node.translation.value()[0],
					node.translation.value()[1],
					node.translation.value()[2]
				});
			}
			if (node.rotation.has_value())
			{
				transform->setOrientation(quat{
					node.rotation.value()[3],
					node.rotation.value()[0],
					node.rotation.value()[1],
					node.rotation.value()[2]
				});
			}
		}

		// create child nodes, setting current node as parent
		if (node.children.size() > 0)
		{
			for (int childIndex : node.children)
			{
				loadNode(nodeEntities, ecs.newEntity(), childIndex, nodeEntity);
			}
		}
	}
	return nodeEntity;
}

void GLTFImporter::createInstance(const Entity &rootEntity) const
{
	// grab the default scene and load the node heirarchy
	const gltf::Scene &scene = model.scenes[model.scene];
	engine.getEcs().createComponent<TransformComponent>(rootEntity);

	// used to map between node indexes (glTF) and entity system IDs
	std::vector<Entity> nodeEntities(model.nodes.size(), Entity::NO_ENTITY);

	if (scene.nodes.size() == 1)
	{
		loadNode(nodeEntities, rootEntity, 0, Entity::NO_ENTITY);
	}
	else
	{
		for (auto &nodeIndex : scene.nodes)
		{
			loadNode(nodeEntities, engine.getEcs().newEntity(), nodeIndex, rootEntity);
		}
	}

	// target -> entity mapping needed for animations
	engine.getEcs().createComponent<AnimationComponent>(rootEntity, nodeEntities);
}
