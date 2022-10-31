#include "core/entitycomponentsystem.h"
#include "physics/movementsystem.h"
#include "core/components/transformcomponent.h"
#include "physics/movementcomponent.h"
#include "camera/cameracomponent.h"
#include "physics/physicscomponent.h"

using namespace Boiler;

MovementSystem::MovementSystem() : System("Movement System")
{
	expects<CameraComponent>();
	expects<MovementComponent>();
	expects<TransformComponent>();
}

void MovementSystem::update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, EntityComponentSystem &ecs)
{
	for (const Entity &entity : getEntities())
	{
        CameraComponent &camera = ecs.getComponentStore().retrieve<CameraComponent>(entity);
        MovementComponent &movement = ecs.getComponentStore().retrieve<MovementComponent>(entity);
        TransformComponent &transform = ecs.getComponentStore().retrieve<TransformComponent>(entity);
        PhysicsComponent &physics = ecs.getComponentStore().retrieve<PhysicsComponent>(entity);

		bool moving = false;
		vec3 strafe(0), move(0);

		// if strafing left and right
		if (movement.moveLeft)
		{
			strafe = -glm::cross(camera.direction, camera.up);
			moving = true;
		}
		else if (movement.moveRight)
		{
			strafe = glm::cross(camera.direction, camera.up);
			moving = true;
		}

		// if moving back/forth
		if (movement.moveForward)
		{
			move = camera.direction;
			moving = true;
		}
		else if (movement.moveBackward)
		{
			move = -camera.direction;
			moving = true;
		}
		vec3 moveDirection = normalize(strafe + move);
		cgfloat acceleration = physics.speed * physics.acceleration;

		if (moving)
		{
			physics.velocity += moveDirection * acceleration;
		}
		else
		{
			physics.velocity *= 0.2f;
			if (length2(physics.velocity) < 0.01 || length2(physics.velocity) > -0.01)
			{
				physics.velocity *= 0;
			}
		}
		transform.setPosition(transform.getPosition() + physics.velocity * frameInfo.deltaTime);
	}
}
