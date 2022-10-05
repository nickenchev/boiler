#ifndef PHYSICSCOMPONENT_H
#define PHYSICSCOMPONENT_H

#include "core/componenttype.h"
#include "core/common.h"
#include "core/math.h"

namespace Boiler {

struct PhysicsComponent : public ComponentType<PhysicsComponent>
{
	cgfloat speed, acceleration;
	vec3 velocity;

	PhysicsComponent() : ComponentType(this)
	{
		speed = 0;
		acceleration = 0;
		velocity = {0, 0, 0};
	}
};

}

#endif /* PHYSICSCOMPONENT_H */
