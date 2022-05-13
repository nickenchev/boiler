#include "core/componentstore.h"
#include "physics/movementsystem.h"
#include "core/components/transformcomponent.h"
#include "physics/movementcomponent.h"

using namespace Boiler;

MovementSystem::MovementSystem() : System("Movement System")
{
	expects<MovementComponent>();
	expects<TransformComponent>();
}

void MovementSystem::update(const FrameInfo &frameInfo, ComponentStore &store)
{
	const float speed = 10.0f;
	for (Entity entity : getEntities())
	{
		MovementComponent &movement = store.retrieve<MovementComponent>(entity);
		TransformComponent &transform = store.retrieve<TransformComponent>(entity);
		vec3 position = transform.getPosition();
		if (movement.moveLeft)
		{
			position.x -= speed * frameInfo.deltaTime;
		}
		else if (movement.moveRight)
		{
			position.x += speed * frameInfo.deltaTime;
		}

		if (movement.moveForward)
		{
			glm::vec3 moveAmount = movement.direction;
			moveAmount *= speed * frameInfo.deltaTime;
			position += moveAmount;
		}
		else if (movement.moveBackward)
		{
			glm::vec3 moveAmount = movement.direction;
			moveAmount *= speed * frameInfo.deltaTime;
			position -= moveAmount;
		}
		transform.setPosition(position);
	}
}
