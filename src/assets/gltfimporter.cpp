#include <filesystem>
#include <fstream>
#include <sstream>

#include "core/engine.h"
#include "core/components/rendercomponent.h"
#include "core/components/positioncomponent.h"
#include "video/vertexdata.h"
#include "assets/gltfimporter.h"

#include "gltf.h"
#include "typedaccessor.h"
#include "modelaccessors.h"

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

using namespace Boiler;

void GLTFImporter::import(Boiler::Engine &engine, std::string gltfPath)
{
	std::ifstream infile(gltfPath);
	std::stringstream buffer;
	buffer << infile.rdbuf();
	const std::string jsonString = buffer.str();
	infile.close();

	auto model = gltf::load(jsonString);

	using namespace std;
	logger.log("Loading model");

	filesystem::path filePath(gltfPath);
	filesystem::path basePath = filePath.parent_path();

	// load all of the buffers
	std::vector<std::vector<std::byte>> buffers;
	for (const auto &buffer : model.buffers)
	{
		buffers.push_back(loadBuffer(basePath.string(), buffer));
	}

	// load materials
	for (size_t i = 0; i < model.materials.size(); ++i)
	{
		Material newMaterial;
		const gltf::Material &material = model.materials[i];
		if (material.pbrMetallicRoughness.value().baseColorTexture.has_value())
		{
			const gltf::MaterialTexture &matTexture = material.pbrMetallicRoughness.value().baseColorTexture.value();
			const gltf::Texture &texture = model.textures[matTexture.index.value()];
			const gltf::Image &image = model.images[texture.source.value()];

			if (image.uri.length() > 0)
			{
				filesystem::path imagePath = basePath;
				imagePath.append(image.uri);
				const ImageData imageData = ImageLoader::load(imagePath.string());

				// load the texture into GPU mem
				auto texture = engine.getRenderer().loadTexture(imagePath, imageData);
				newMaterial.baseTexture = texture;
			}
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
		newMaterial.color = vec4(1, 1, 1, 1);
		MaterialId materialId = engine.getRenderSystem().addMaterial(newMaterial);
		materialIds.push_back(materialId);
	}

	// Model accessors which are used for typed access into buffers
	gltf::ModelAccessors modelAccess(model, std::move(buffers));

	// grab the default scene and load the node heirarchy
	const gltf::Scene &scene = model.scenes[model.scene];
	std::unordered_map<int, Entity> nodeEntities;
	for (auto &nodeIndex : scene.nodes)
	{
		loadNode(engine, model, modelAccess, nodeEntities, nodeIndex);
	}
}

auto GLTFImporter::loadPrimitive(Engine &engine, const gltf::ModelAccessors &modelAccess, const gltf::Primitive &primitive)
{
	if (primitive.mode.has_value())
	{
		assert(primitive.mode == 4);
	}
	EntityComponentSystem &ecs = engine.getEcs();
	using namespace gltf::attributes;

	// get the primitive's position data
	std::vector<Vertex> vertices;
	auto positionAccess = modelAccess.getTypedAccessor<float, 3>(primitive, POSITION);
	for (auto values : positionAccess)
	{
		Vertex vertex({values[0], -values[1], values[2]});
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
	if (primitive.indices.has_value())
	{
		const auto &indexAccessor = modelAccess.getModel().accessors[primitive.indices.value()];
		if (indexAccessor.componentType == 5123)
		{
			auto indexAccess = modelAccess.getTypedAccessor<unsigned short, 1>(primitive, primitive.indices.value());
			for (auto values : indexAccess)
			{
				indices.push_back(values[0]);
			}
		}
		else if (indexAccessor.componentType == 5125)
		{
			auto indexAccess = modelAccess.getTypedAccessor<unsigned int, 1>(primitive, primitive.indices.value());
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
	if (primitive.attributes.find(TEXCOORD_0) != primitive.attributes.end())
	{
		const auto &accessor = modelAccess.getModel().accessors[primitive.attributes.find(TEXCOORD_0)->second];
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

Entity GLTFImporter::loadNode(Engine &engine, const gltf::Model &model, const gltf::ModelAccessors &modelAccess,
							  std::unordered_map<int, Entity> &nodeEntities, int nodeIndex)
{
	EntityComponentSystem &ecs = engine.getEcs();
	Entity nodeEntity = ecs.newEntity();

	const gltf::Node &node = model.nodes[nodeIndex];
	logger.log("Loading node: {}", node.name);

	if (node.mesh.has_value())
	{
		const auto &mesh = model.meshes[node.mesh.value()];
		logger.log("Loading mesh: {}", mesh.name);

		auto renderComp = ecs.createComponent<RenderComponent>(nodeEntity);
		for (auto &gltfPrimitive : mesh.primitives)
		{
			Primitive meshPrimitive = loadPrimitive(engine, modelAccess, gltfPrimitive);
			// setup material if any
			if (gltfPrimitive.material.has_value())
			{
				const MaterialId materialId = materialIds[gltfPrimitive.material.value()];
				meshPrimitive.materialId = materialId;
			}
			renderComp->mesh.primitives.push_back(meshPrimitive);
		}
	}
	auto renderPos = ecs.createComponent<PositionComponent>(nodeEntity, Rect(0, 0, 0, 0));

	// decompose a matrix if available, otherwise try to load transformations directly
	if (node.matrix.has_value())
	{
		const auto &matrixArray = node.matrix.value();
		mat4 matrix = glm::make_mat4(matrixArray.data());
		vec3 scale, skew, position;
		glm::quat orientation;
		vec4 perspective;

		glm::decompose(matrix, scale, orientation, position, skew, perspective);
		renderPos->frame.position = position;
		renderPos->scale = scale;
		renderPos->orientation = glm::conjugate(orientation); // TODO: https://stackoverflow.com/questions/17918033/glm-decompose-mat4-into-translation-and-rotation
	}
	else
	{
		// otherwise load transformation directly
		if (node.scale.has_value())
		{
			renderPos->scale = {
				node.scale.value()[0],
				node.scale.value()[1],
				node.scale.value()[2]
			};
		}
		if (node.translation.has_value())
		{
			renderPos->frame.position = {
				node.translation.value()[0],
				-node.translation.value()[1],
				node.translation.value()[2]
			};
		}
		if (node.rotation.has_value())
		{
			renderPos->orientation.x = node.rotation.value()[0];
			renderPos->orientation.y = node.rotation.value()[1];
			renderPos->orientation.z = node.rotation.value()[2];
			renderPos->orientation.w = node.rotation.value()[3];
		}
	}

	// if this node has children, create them and assign the current node as parent
	if (node.children.size() > 0)
	{
		for (int childIndex : node.children)
		{
			Entity childEntity = loadNode(engine, model, modelAccess, nodeEntities, childIndex);
			ecs.createComponent<ParentComponent>(childEntity, nodeEntity);
		}
	}

	return nodeEntity;
}
