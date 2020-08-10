#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H

#include "core/math.h"

namespace Boiler
{
	
struct LightSource
{
	vec4 position;
	vec4 color;

	LightSource()
	{
		this->position = {0, 0, 0, 0};
		this->color = {1, 1, 1, 1};
	}

	LightSource(vec3 position, vec3 color)
	{
		this->position = vec4(position, 1);
		this->color = vec4(color, 1);
	}
};

}

#endif
