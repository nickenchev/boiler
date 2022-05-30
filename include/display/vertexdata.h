#ifndef VERTEXDATA_H
#define VERTEXDATA_H

#include <vector>
#include <array>

#include "core/vertex.h"

namespace Boiler
{

class VertexData
{
    std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

public:
	VertexData() { }

	VertexData(const std::vector<Vertex> &vertices, const std::vector<uint32_t> &indices) : vertices(vertices), indices(indices)
	{
	}

    const float *vertexBegin() const { return &(vertices[0].position.x); }
    const uint32_t *indexBegin() const { return &(indices[0]); }

	auto &vertexArray() const { return vertices; }
	long vertexByteSize() const { return vertices.size() * sizeof(Vertex); }
	auto &indexArray() const { return indices; }
	long indexByteSize() const { return indices.size() * sizeof(uint32_t); }
};

}

#endif /* VERTEXDATA_H */
