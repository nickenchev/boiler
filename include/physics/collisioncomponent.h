#ifndef COLLISIONCOMPONENT_H
#define COLLISIONCOMPONENT_H

#include "core/componenttype.h"

namespace Boiler
{

struct CollisionComponent : public ComponentType<CollisionComponent>
{
	CollisionComponent() : ComponentType(this) {}
};

}

#endif // !COLLISIONCOMPONENT_H
