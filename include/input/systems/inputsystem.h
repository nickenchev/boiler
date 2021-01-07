#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H

#include "core/system.h"

namespace Boiler
{

class InputSystem : public System
{
public:
	void update(ComponentStore &store, const Time delta) const override
	{
	}
};

}

#endif /* INPUTSYSTEM_H */
