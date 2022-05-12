#ifndef INPUTCOMPONENT_H
#define INPUTCOMPONENT_H

#include "core/componenttype.h"

namespace Boiler
{

struct InputComponent : public ComponentType<InputComponent>
{
	bool moveLeft, moveRight, moveForward, moveBackward, moveUp, moveDown;

public:
	InputComponent() : ComponentType(this)
	{
	}
};

}

#endif /* INPUTCOMPONENT_H */
