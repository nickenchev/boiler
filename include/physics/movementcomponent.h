#ifndef MOVEMENTCOMPONENT_H
#define MOVEMENTCOMPONENT_H

#include "core/componenttype.h"
#include "core/math.h"

namespace Boiler
{

struct MovementComponent : public ComponentType<MovementComponent>
{
	bool moveLeft, moveRight, moveForward, moveBackward, moveUp, moveDown, jump;
	float mouseXDiff, mouseYDiff;

	MovementComponent() : ComponentType(this)
	{
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
