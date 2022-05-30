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

void MovementSystem::update(AssetSet &assetSet, const FrameInfo &frameInfo, ComponentStore &store)
{
	const float gravity = 15.0f;
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

		// strifing mid-air slower than on the ground
		float slowdown = 0.6f;
		if (movement.moveLeft)
		{
			glm::vec3 moveAmount = glm::cross(movement.direction, movement.up);
			moveAmount *= frameInfo.deltaTime * speed * slowdown;
			position -= moveAmount;
		}
		else if (movement.moveRight)
		{
			glm::vec3 moveAmount = glm::cross(movement.direction, movement.up);
			moveAmount *= frameInfo.deltaTime * speed * slowdown;
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
