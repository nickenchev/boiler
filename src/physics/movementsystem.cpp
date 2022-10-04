#include "core/componentstore.h"
#include "physics/movementsystem.h"
#include "core/components/transformcomponent.h"
#include "physics/movementcomponent.h"
#include "physics/physicscomponent.h"

using namespace Boiler;

MovementSystem::MovementSystem() : System("Movement System")
{
	expects<MovementComponent>();
	expects<TransformComponent>();
}

void MovementSystem::update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, ComponentStore &store)
{
	const float speed = 3.0f;

	for (const Entity &entity : getEntities())
	{
		MovementComponent &movement = store.retrieve<MovementComponent>(entity);
		TransformComponent &transform = store.retrieve<TransformComponent>(entity);
		PhysicsComponent &physics = store.retrieve<PhysicsComponent>(entity);

		// strifing mid-air slower than on the ground
		float slowdown = 0.6f;
		vec3 acceleration(0, 0, 0);
		if (movement.moveLeft)
		{
			glm::vec3 moveAmount = glm::cross(movement.direction, movement.up);
			moveAmount *= frameInfo.deltaTime * speed * slowdown;
			acceleration = -moveAmount;
		}
		else if (movement.moveRight)
		{
			glm::vec3 moveAmount = glm::cross(movement.direction, movement.up);
			moveAmount *= frameInfo.deltaTime * speed * slowdown;
			acceleration = moveAmount;
		}

		if (movement.moveForward)
		{
			//glm::vec3 moveAmount = vec3(movement.direction.x, 0, movement.direction.z);
			glm::vec3 moveAmount = movement.direction;
			moveAmount *= speed * frameInfo.deltaTime;
			acceleration += moveAmount;
		}
		else if (movement.moveBackward)
		{
			//glm::vec3 moveAmount = vec3(movement.direction.x, 0, movement.direction.z);
			glm::vec3 moveAmount = movement.direction;
			moveAmount *= speed * frameInfo.deltaTime;
			acceleration += -moveAmount;
		}

		if (acceleration.x + acceleration.y + acceleration.z != 0)
		{
			physics.velocity += acceleration;
		}
		else
		{
			physics.velocity *= 0.2f;
			if (physics.velocity.length() < 0.01)
			{
				physics.velocity *= 0;
			}
		}
	}
}
