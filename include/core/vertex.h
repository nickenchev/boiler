#ifndef VERTEX_H
#define VERTEX_H

#include "core/math.h"

namespace Boiler {

struct Vertex
{
	glm::vec3 position;
	glm::vec4 colour;
	glm::vec2 textureCoordinates;
	glm::vec3 normal;

	Vertex()
	{
	}

	Vertex(vec3 position)
	{
		this->position = position;
	}

	Vertex(vec3 position, vec2 textureCoordinates)
	{
		this->position = position;
		this->textureCoordinates = textureCoordinates;
	}

	Vertex(vec3 position, vec2 textureCoordinates, vec4 colour)
	{
		this->position = position;
		this->textureCoordinates = textureCoordinates;
		this->colour = colour;
	}
};

}
#endif /* VERTEX_H */
