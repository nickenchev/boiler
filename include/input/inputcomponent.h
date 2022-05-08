#ifndef INPUTCOMPONENT_H
#define INPUTCOMPONENT_H

#include "core/componenttype.h"

namespace Boiler
{

class InputComponent : public ComponentType<InputComponent>
{
public:
	InputComponent() : ComponentType(this)
	{

	}
};

}

#endif /* INPUTCOMPONENT_H */
