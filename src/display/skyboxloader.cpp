#include <array>

#include "core/engine.h"
#include "display/renderer.h"
#include "display/skyboxloader.h"
#include "display/vertexdata.h"
#include "display/imaging/imageloader.h"
#include "core/components/rendercomponent.h"
#include "core/components/transformcomponent.h"

using namespace Boiler;

Entity SkyBoxLoader::load(const std::string &top, const std::string &bottom,
						  const std::string &left, const std::string &right,
						  const std::string &front, const std::string &back)
{
	std::array<ImageData, 6> images{
		ImageLoader::load(right),
		ImageLoader::load(left),
		ImageLoader::load(top),
		ImageLoader::load(bottom),
		ImageLoader::load(front),
		ImageLoader::load(back)
	};

	vec3 min(-1, -1, -1);
	vec3 max(1, 1, 1);
	float skyboxVertices[] = {
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	const unsigned int numVerts = 36;
	// iterate all vertices
	int stepSize = 3;
	for (unsigned int i = 0; i < numVerts; ++i)
	{
		bool found = false;
		for (unsigned int j = 0; j < static_cast<unsigned int>(vertices.size()); ++j)
		{
			if (vertices[j].position.x == skyboxVertices[i * stepSize] &&
				vertices[j].position.y == skyboxVertices[i * stepSize + 1] &&
				vertices[j].position.z == skyboxVertices[i * stepSize + 2])
			{
				indices.push_back(j);
				found = true;
			}
		}
		if (!found)
		{
			vec3 position(skyboxVertices[i * stepSize], skyboxVertices[i * stepSize + 1], skyboxVertices[i * stepSize + 2]);
			Vertex vertex(position);
			vertex.colour = vec4(1, 1, 1, 1);

			vertices.push_back(vertex);
			indices.push_back(vertices.size() - 1);
		}
	}

	Material material;
	material.baseTexture = renderer.loadCubemap(images);
	material.depth = false; // TODO: TEMP
	material.diffuse = vec4(1, 1, 1, 1);
	AssetId matAssetId = assetSet.materials.add(std::move(material));

	VertexData vertexData(vertices, indices);
	AssetId bufferId = renderer.loadPrimitive(vertexData);
	Primitive primitive(bufferId, std::move(vertexData), min, max);
	primitive.materialId = matAssetId;
	AssetId primitiveId = assetSet.primitives.add(std::move(primitive));

	Entity skyBox = ecs.newEntity();
	auto renderComponent = ecs.createComponent<RenderComponent>(skyBox);
	renderComponent->mesh.primitives.push_back(primitiveId);
	ecs.createComponent<TransformComponent>(skyBox);

	return skyBox;
}
