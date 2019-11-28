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
};

class VertexData
{
    bool owned;
    std::vector<Vertex> vertices;

public:
	VertexData(const std::vector<Vertex> &vertices) : vertices(vertices)
	{
	}

    const float *begin() const { return &(vertices[0].position.x); }
    int length() const { return vertices.size(); }
    int size() const { return vertices.size() * sizeof(Vertex); }
};

}

#endif /* VERTEXDATA_H */
