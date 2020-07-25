#ifndef LIGHTINGCOMPONENT_H
#define LIGHTINGCOMPONENT_H

#include "core/componenttype.h"
#include "video/lightsource.h"

namespace Boiler
{
	
struct LightingComponent : public ComponentType<LightingComponent>
{
	LightSource lightSource;
};

}

#endif
