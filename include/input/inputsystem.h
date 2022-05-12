#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H

#include "core/system.h"

namespace Boiler
{

class Engine;

class InputSystem : public System
{
	bool moveLeft, moveRight, moveForward, moveBackward, moveUp, moveDown;
	
public:
    InputSystem(Engine &engine);
	void update(const FrameInfo &frameInfo, ComponentStore &store) override;
};

}
#endif /* INPUTSYSTEM_H */
