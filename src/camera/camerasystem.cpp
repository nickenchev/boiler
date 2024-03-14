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
			transform.translation += physics.velocity;
			camera.direction = glm::rotate(camera.direction, static_cast<float>(-movement.mouseXDiff * 2), camera.up);
			const glm::vec3 axis = glm::cross(camera.up, camera.direction);
			camera.direction = glm::rotate(camera.direction, static_cast<float>(movement.mouseYDiff * 2), axis);

			glm::mat4 view = glm::lookAt(transform.translation, transform.translation + camera.direction, camera.up);
			renderer.setViewMatrix(view);
			renderer.setCameraPosition(transform.translation);
		}
		else if (camera.type == CameraType::arcball)
		{
			vec3 position = glm::rotate(transform.translation, transform.orientation.y * frameInfo.deltaTime, camera.up);
			vec3 forward = glm::normalize(camera.direction - position); // get forward vector, direction = target
			vec3 right = glm::cross(camera.up, forward);
			vec3 finalPosition = glm::rotate(position, transform.orientation.x * frameInfo.deltaTime, right);

			glm::mat4 view = glm::lookAt(finalPosition, camera.direction, camera.up);
			renderer.setViewMatrix(view);
			renderer.setCameraPosition(finalPosition);
		}
	}
}
