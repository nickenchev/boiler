#include "core/engine.h"
#include "display/renderer.h"
#include "input/inputsystem.h"
#include "input/inputcomponent.h"
#include "input/keyinputevent.h"
#include "core/entitycomponentsystem.h"
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
	prevXFactor = prevYFactor = 0;
}

void InputSystem::update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, EntityComponentSystem &ecs)
{
	bool jump = false;
	
	for (unsigned int i = 0; i < frameInfo.keyInputEvents.getCount(); ++i)
	{
		const KeyInputEvent event = frameInfo.keyInputEvents[i];
		switch (event.keyCode)
		{
			case KeyCode::a:
			{
				moveLeft = event.state == ButtonState::DOWN;
				break;
			}
			case KeyCode::d:
			{
				moveRight = event.state == ButtonState::DOWN;
				break;
			}
			case KeyCode::s:
			{
				moveBackward = event.state == ButtonState::DOWN;
				break;
			}
			case KeyCode::w:
			{
				moveForward = event.state == ButtonState::DOWN;
				break;
			}
		}
	}

	// calculate mouse diff
	const Size size = engine.getRenderer().getScreenSize();
	const float xFactorNew = frameInfo.mouseXDiff / size.width;
	const float yFactorNew = frameInfo.mouseYDiff / size.height;

	const float alpha = 0.5f;
	const float xFactor = prevXFactor + alpha * (xFactorNew - prevXFactor);
	const float yFactor = prevYFactor + alpha * (yFactorNew - prevYFactor);

	const float sensitivity = 1.5;
	const float xDiff = sensitivity * xFactor;
	const float yDiff = sensitivity * yFactor;

	prevXFactor = xFactorNew;
	prevYFactor = yFactorNew;

	for (const Entity &entity : getEntities())
	{
		MovementComponent &movement = ecs.getComponentStore().retrieve<MovementComponent>(entity);
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
