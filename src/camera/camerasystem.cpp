#include <glm/gtx/rotate_vector.hpp>

#include "display/renderer.h"
#include "core/entitycomponentsystem.h"
#include "camera/camerasystem.h"
#include "core/components/transformcomponent.h"
#include "physics/movementcomponent.h"
#include "camera/cameracomponent.h"
#include "physics/physicscomponent.h"

using namespace Boiler;

CameraSystem::CameraSystem() : System("Camera System")
{
	expects<TransformComponent>();
	expects<CameraComponent>();
	expects<MovementComponent>();
	expects<PhysicsComponent>();

	prevXFactor = 0;
	prevYFactor = 0;
}

void CameraSystem::update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, EntityComponentSystem &ecs)
{
	for (const Entity &entity : getEntities())
	{
        TransformComponent &transform = ecs.retrieve<TransformComponent>(entity);
        CameraComponent &camera = ecs.retrieve<CameraComponent>(entity);
        MovementComponent &movement = ecs.retrieve<MovementComponent>(entity);
		PhysicsComponent& physics = ecs.retrieve<PhysicsComponent>(entity);

		if (camera.type == CameraType::firstPerson)
		{
			const int mouseSpeed = 100;

			/*
			camera.forward = glm::rotate(camera.forward, movement.mouseXDiff * mouseSpeed * frameInfo.deltaTime, -camera.up);
			glm::vec3 right = glm::cross(camera.up, camera.forward);
			camera.forward = glm::rotate(camera.forward, movement.mouseYDiff * mouseSpeed * frameInfo.deltaTime, right);

			const float speed = 10.0f;
			vec3 moveAmount(0);

			if (movement.moveForward)
			{
				moveAmount += camera.forward * speed * frameInfo.deltaTime;
			}
			if (movement.moveBackward)
			{
				moveAmount -= camera.forward * speed * frameInfo.deltaTime;
			}
			if (movement.moveLeft)
			{
				moveAmount +=  right * speed * frameInfo.deltaTime;
			}
			if (movement.moveRight)
			{
				moveAmount -= right * speed * frameInfo.deltaTime;
			}

			transform.translation += moveAmount;

			glm::mat4 view = glm::lookAt(transform.translation, transform.translation + camera.forward, camera.up);
			renderer.setViewMatrix(view);
			renderer.setCameraPosition(transform.translation);
			*/
		}
		else if (camera.type == CameraType::arcball)
		{
			const vec3 worldUp(0, 1, 0);
			const float yaw = -frameInfo.mouseXDiff * 0.5f * frameInfo.deltaTime;
			const float pitch = frameInfo.mouseYDiff * 0.5f * frameInfo.deltaTime;

			camera.offset = glm::rotate(camera.offset, yaw, worldUp);
			camera.up = glm::rotate(camera.up, yaw, worldUp);
			vec3 forward = glm::normalize(-camera.offset);
			vec3 right = glm::normalize(glm::cross(camera.up, forward));
			camera.offset = glm::rotate(camera.offset, pitch, right);
			camera.up = glm::rotate(camera.up, pitch, right);

			vec3 target = transform.translation;
			vec3 cameraPos = target + camera.offset;

			mat4 view = glm::lookAt(cameraPos, target, camera.up);
			renderer.setViewMatrix(view);
			renderer.setCameraPosition(cameraPos);
		}
	}
}
