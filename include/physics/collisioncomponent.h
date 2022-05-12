#ifndef COLLISIONCOMPONENT_H
#define COLLISIONCOMPONENT_H

#include "core/componenttype.h"
#include "core/common.h"

namespace Boiler
{

struct CollisionComponent : public ComponentType<CollisionComponent>
{
	vec3 min, max;
	CollisionComponent() : ComponentType(this) { }
};

}

#endif // !COLLISIONCOMPONENT_H
