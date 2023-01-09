#ifndef LIGHTINGCOMPONENT_H
#define LIGHTINGCOMPONENT_H

#include "core/componenttype.h"
#include "display/lightsource.h"

namespace Boiler
{
	
struct LightingComponent : public ComponentType<LightingComponent>
{
	vec4 color;

	LightingComponent(const vec4 &color) : ComponentType(this)
	{
		this->color = color;
	}
};

}

#endif
