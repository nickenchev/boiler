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
	const float ground = 1.1f;
	const float gravity = 12.0f;
	const float speed = 10.0f;

	for (Entity entity : getEntities())
	{
		MovementComponent &movement = store.retrieve<MovementComponent>(entity);
		TransformComponent &transform = store.retrieve<TransformComponent>(entity);

		// apply gravity
		mat4 transformMatrix = transform.getMatrix();
		vec3 position = transform.getPosition();
		vec3 velocity = transform.getVelocity();

		velocity.y -= gravity * frameInfo.deltaTime;

		if (position.y < ground)
		{
			position.y = ground;
			velocity.y = -velocity.y * 0.1f;
		}
		if (movement.moveLeft)
		{
			glm::vec3 moveAmount = glm::cross(movement.direction, movement.up);
			moveAmount *= frameInfo.deltaTime * speed * 0.6f;
			position -= moveAmount;
		}
		else if (movement.moveRight)
		{
			glm::vec3 moveAmount = glm::cross(movement.direction, movement.up);
			moveAmount *= frameInfo.deltaTime * speed * 0.6f;
			position += moveAmount;
		}

		if (movement.moveForward)
		{
			glm::vec3 moveAmount = vec3(movement.direction.x, 0, movement.direction.z);
			moveAmount *= speed * frameInfo.deltaTime;
			position += moveAmount;
		}
		else if (movement.moveBackward)
		{
			glm::vec3 moveAmount = vec3(movement.direction.x, 0, movement.direction.z);
			moveAmount *= speed * frameInfo.deltaTime;
			position -= moveAmount;
		}

		if (movement.jump)
		{
			if (position.y < 2)
			{
				velocity.y += 7.0f;
			}
		}

		position += velocity * frameInfo.deltaTime;
		transform.setVelocity(velocity);
		transform.setPosition(position);
	}
}
