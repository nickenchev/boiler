#ifndef MOVEMENTCOMPONENT_H
#define MOVEMENTCOMPONENT_H

#include "core/componenttype.h"
#include "core/common.h"

namespace Boiler
{

struct MovementComponent : public ComponentType<MovementComponent>
{
	bool moveLeft, moveRight, moveForward, moveBackward, moveUp, moveDown, jump;
	float mouseXDiff, mouseYDiff;
	vec3 direction, up;

	MovementComponent() : ComponentType(this)
	{
		direction = {0, 0, 0};
		up = {0, 0, 0};
		moveLeft = false;
		moveRight = false;
		moveForward = false;
		moveBackward = false;
		moveUp = false;
		moveDown = false;
		jump = false;
		mouseXDiff = 0;
		mouseYDiff = 0;
	}
};

}

#endif /* MOVEMENTCOMPONENT_H */
