#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H

#include "core/math.h"

namespace Boiler
{
	
struct LightSource
{
	vec3 position;
	vec3 color;

	LightSource()
	{
		position = {0, 0, 0};
		color = {1, 1, 1};
	}
};

}

#endif
