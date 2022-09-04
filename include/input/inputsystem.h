#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H

#include "core/system.h"

namespace Boiler
{

class Engine;

class InputSystem : public System
{
	Engine &engine;
	bool moveLeft, moveRight, moveForward, moveBackward, moveUp, moveDown;
	float prevXFactor, prevYFactor;
	
public:
    InputSystem(Engine &engine);
	void update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, ComponentStore &store) override;
};

}
#endif /* INPUTSYSTEM_H */
