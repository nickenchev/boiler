#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H

#include "core/system.h"
#define KEY_STATES_SIZE 256

namespace Boiler
{

class Engine;

class InputSystem : public System
{
	ButtonState keyStates[KEY_STATES_SIZE];
	Engine &engine;
	bool moveLeft, moveRight, moveForward, moveBackward, moveUp, moveDown;
	float prevXFactor, prevYFactor;
	
public:
    InputSystem(Engine &engine);
    void update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, EntityComponentSystem &ecs) override;
};

}
#endif /* INPUTSYSTEM_H */
