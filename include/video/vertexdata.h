#ifndef VERTEXDATA_H
#define VERTEXDATA_H

#include <vector>
#include <array>
#include "core/math.h"

namespace Boiler
{

class VertexData
{
    bool owned;
    std::vector<vec3> vertices;

public:
    VertexData(const std::vector<vec3> &vertices);

	template<size_t SIZE>
	VertexData(std::array<float, SIZE> vertices)
	{
		const size_t vertexCount = vertices.size();
		if (vertexCount % 3 > 0)
		{
			throw std::runtime_error("Invalid vertex data passed, count must be divisible by 3 (x, y, z)");
		}

		const short vertDataCount = 3;
		this->vertices.resize(vertexCount / vertDataCount);

		for (int i = 0; i < vertexCount; i += vertDataCount)
		{
			this->vertices.push_back(glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]));
		}

		owned = false;
	}

    const float *begin() const { return &(vertices[0].x); }
    int length() const { return vertices.size(); }
    int size() const { return vertices.size() * sizeof(vec3); }
};

}

#endif /* VERTEXDATA_H */
