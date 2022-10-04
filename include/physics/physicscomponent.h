#ifndef PHYSICSCOMPONENT_H
#define PHYSICSCOMPONENT_H

#include "core/componenttype.h"
#include "core/math.h"

namespace Boiler {

struct PhysicsComponent : public ComponentType<PhysicsComponent>
{
	vec3 acceleration, velocity;

	PhysicsComponent() : ComponentType(this)
	{
		acceleration = {0, 0, 0};
		velocity = {0, 0, 0};
	}
};

}

#endif /* PHYSICSCOMPONENT_H */
