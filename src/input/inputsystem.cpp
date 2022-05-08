#include "core/engine.h"
#include "input/inputsystem.h"
#include "input/inputcomponent.h"
#include "input/keyinputevent.h"
#include "core/components/transformcomponent.h"
#include "core/componentstore.h"

using namespace Boiler;

InputSystem::InputSystem(Engine &engine) : System("Input System")
{
	expects<InputComponent>();

	moveLeft = false;
	moveRight = false;
	moveForward = false;
	moveBackward = false;
	moveUp = false;
	moveDown = false;

	/*
    auto keyListener = [this](const KeyInputEvent &event)
	{
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
		else if (event.keyCode == SDLK_PAGEUP)
		{
			moveUp = event.state == ButtonState::DOWN;
		}
		else if (event.keyCode == SDLK_PAGEDOWN)
		{
			moveDown = event.state == ButtonState::DOWN;
		}
		else if (event.keyCode == SDLK_r && event.state == ButtonState::UP)
		{
			//engine.getAnimator().resetTime();
		}
	};
	engine.addKeyInputListener(keyListener);
	*/
}

void InputSystem::update(const FrameInfo &frameInfo, ComponentStore &store)
{
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

	const float speed = 10.0f;
	for (Entity entity : getEntities())
	{
		TransformComponent &transform = store.retrieve<TransformComponent>(entity);
		vec3 position = transform.getPosition();
		if (moveLeft)
		{
			position.x -= speed * frameInfo.deltaTime;
		}
		else if (moveRight)
		{
			position.x += speed * frameInfo.deltaTime;
		}

		/*
		if (moveForward)
		{
			glm::vec3 moveAmount = camDirection;
			moveAmount *= speed * deltaTime;
			camPosition -= moveAmount;
		}
		else if (moveFurther)
		{
			glm::vec3 moveAmount = camDirection;
			moveAmount *= speed * deltaTime;
			camPosition += moveAmount;
		}

		if (moveUp)
		{
			camPosition.y += speed * deltaTime;
		}
		else if (moveDown)
		{
			camPosition.y -= speed * deltaTime;
		}
		*/
		transform.setPosition(position);
	}
}
