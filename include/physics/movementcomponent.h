#ifndef MOVEMENTCOMPONENT_H
#define MOVEMENTCOMPONENT_H

#include "core/componenttype.h"

namespace Boiler
{

struct MovementComponent : public ComponentType<MovementComponent>
{
	bool moveLeft, moveRight, moveForward, moveBackward, moveUp, moveDown;

	MovementComponent() : ComponentType(this)
	{
		moveLeft = false;
		moveRight = false;
		moveForward = false;
		moveBackward = false;
		moveUp = false;
		moveDown = false;
	}
};

}

#endif /* MOVEMENTCOMPONENT_H */
