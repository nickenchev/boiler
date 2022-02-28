#ifndef LIGHTINGCOMPONENT_H
#define LIGHTINGCOMPONENT_H

#include "core/componenttype.h"
#include "display/lightsource.h"

namespace Boiler
{
	
struct LightingComponent : public ComponentType<LightingComponent>
{
	LightSource source;

	LightingComponent(const LightSource &source) : ComponentType(this), source(source)
	{
	}
};

}

#endif
