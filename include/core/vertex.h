#ifndef VERTEX_H
#define VERTEX_H

#include "core/math.h"

namespace Boiler {

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

}
#endif /* VERTEX_H */
