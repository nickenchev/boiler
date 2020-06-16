#ifndef VERTEXDATA_H
#define VERTEXDATA_H

#include <vector>
#include <array>
#include "core/math.h"

namespace Boiler
{

struct Vertex
{
	glm::vec3 position;
	glm::vec4 colour;
	glm::vec2 textureCoordinate;
	glm::vec3 normal;

	Vertex(glm::vec3 position)
	{
		this->position = position;
	}
};

class VertexData
{
    bool owned;
    std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

public:
	VertexData(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices) : vertices(vertices), indices(indices)
	{
	}

    const float *vertexBegin() const { return &(vertices[0].position.x); }
    const uint32_t *indexBegin() const { return &(indices[0]); }

	auto &vertexArray() const { return vertices; }
	long vertexSize() const { return vertices.size() * sizeof(Vertex); }
	auto &indexArray() const { return indices; }
	long indexSize() const { return indices.size() * sizeof(uint32_t); }
};

}

#endif /* VERTEXDATA_H */
