#include "core/engine.h"
#include "display/renderer.h"
#include "input/inputsystem.h"
#include "input/inputcomponent.h"
#include "input/keyinputevent.h"
#include "core/entitycomponentsystem.h"
#include "physics/movementcomponent.h"

using namespace Boiler;

InputSystem::InputSystem(Engine &engine) : System("Input System"), engine(engine), keyStates{ ButtonState::NONE }
{
	expects<InputComponent>();
	expects<MovementComponent>();

	moveLeft = false;
	moveRight = false;
	moveForward = false;
	moveBackward = false;
	moveUp = false;
	moveDown = false;
	prevXFactor = prevYFactor = 0;
	mouseXDiff = mouseYDiff = 0;
}

void InputSystem::update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, EntityComponentSystem &ecs)
{
	if (keyEvents.getCount())
	{
		for (unsigned int i = 0; i < keyEvents.getCount(); ++i)
		{
			const KeyInputEvent event = keyEvents[i];
			if (event.keyCode < KEY_STATES_SIZE)
			{
				keyStates[event.keyCode] = event.state;
			}
		}
		keyEvents.reset();
	}

	moveLeft = keyStates['A'] == ButtonState::DOWN;
	moveRight = keyStates['D'] == ButtonState::DOWN;
	moveForward = keyStates['W'] == ButtonState::DOWN;
	moveBackward = keyStates['S'] == ButtonState::DOWN;

	for (const Entity &entity : getEntities())
	{
		MovementComponent &movement = ecs.getComponentStore().retrieve<MovementComponent>(entity);
		movement.moveLeft = moveLeft;
		movement.moveRight = moveRight;
		movement.moveForward = moveForward;
		movement.moveBackward = moveBackward;
		movement.moveUp = moveUp;
		movement.moveDown = moveDown;
	}
}

void Boiler::InputSystem::addKeyEvent(const KeyInputEvent &event)
{
	keyEvents.addEvent(event);
}
