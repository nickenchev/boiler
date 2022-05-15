#include "core/engine.h"
#include "input/inputsystem.h"
#include "input/inputcomponent.h"
#include "input/keyinputevent.h"
#include "core/componentstore.h"
#include "physics/movementcomponent.h"

using namespace Boiler;

InputSystem::InputSystem(Engine &engine) : System("Input System"), engine(engine)
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
	bool jump = false;
	
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
		else if (event.keyCode == SDLK_SPACE)
		{
			jump = event.state == ButtonState::DOWN;
		}
	}

	// calculate mouse diff
	const Size size = engine.getRenderer().getScreenSize();
	const float xFactorNew = frameInfo.mouseXDistance / size.width;
	const float yFactorNew = frameInfo.mouseYDistance / size.height;

	const float alpha = 0.5f;
	const float xFactor = prevXFactor + alpha * (xFactorNew - prevXFactor);
	const float yFactor = prevYFactor + alpha * (yFactorNew - prevYFactor);

	const float sensitivity = 1.0;
	const float xDiff = sensitivity * xFactor;
	const float yDiff = sensitivity * yFactor;

	prevXFactor = xFactorNew;
	prevYFactor = yFactorNew;

	for (Entity entity : getEntities())
	{
		MovementComponent &movement = store.retrieve<MovementComponent>(entity);
		movement.moveLeft = moveLeft;
		movement.moveRight = moveRight;
		movement.moveForward = moveForward;
		movement.moveBackward = moveBackward;
		movement.moveUp = moveUp;
		movement.moveDown = moveDown;
		movement.jump = jump;
		movement.mouseXDiff = xDiff;
		movement.mouseYDiff = yDiff;
	}
}
