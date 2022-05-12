#include "core/engine.h"
#include "input/inputsystem.h"
#include "input/inputcomponent.h"
#include "input/keyinputevent.h"
#include "core/componentstore.h"
#include "physics/movementcomponent.h"

using namespace Boiler;

InputSystem::InputSystem(Engine &engine) : System("Input System")
{
	expects<InputComponent>();
	expects<MovementComponent>();

	moveLeft = false;
	moveRight = false;
	moveForward = false;
	moveBackward = false;
	moveUp = false;
	moveDown = false;
}

void InputSystem::update(const FrameInfo &frameInfo, ComponentStore &store)
{
	bool moveLeft, moveRight, moveForward, moveBackward, moveUp, moveDown;

	for (unsigned int i = 0; i < frameInfo.keyInputEvents.getCount(); ++i)
	{
		const KeyInputEvent event = frameInfo.keyInputEvents[i];
		if (event.keyCode == SDLK_a)
		{
			moveLeft = event.state == ButtonState::DOWN;
		}
		else if (event.keyCode == SDLK_d)
		{
			moveRight = event.state == ButtonState::DOWN;
		}
		else if (event.keyCode == SDLK_s)
		{
			moveBackward = event.state == ButtonState::DOWN;
		}
		else if (event.keyCode == SDLK_w)
		{
			moveForward = event.state == ButtonState::DOWN;
		}
	}

	for (Entity entity : getEntities())
	{
		MovementComponent &movement = store.retrieve<MovementComponent>(entity);
		movement.moveLeft = moveLeft;
		movement.moveRight = moveRight;
		moveForward = moveForward;
		moveBackward = moveBackward;
		moveUp = moveUp;
		moveDown = moveDown;
	}
}
