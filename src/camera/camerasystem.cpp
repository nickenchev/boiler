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
			// amount to rotate this frame
			float pitchAmount = transform.orientation.x * frameInfo.deltaTime * 0.2f;
			float c = cos(pitchAmount); // used to figure out if we've rotated the cam upside-down
			float yawAmount = transform.orientation.y * frameInfo.deltaTime * 0.2f;
			vec3 camYAxis = c > 0 ? camera.up : -camera.up;

			vec3 forward = glm::normalize(camera.direction - transform.translation);
			vec3 right = glm::cross(camera.up, forward);
			mat3 xRot = glm::rotate(pitchAmount, right);
			vec3 newPosition = xRot * transform.translation;

			mat3 yRot = glm::rotate(yawAmount, camera.up);
			newPosition = yRot * newPosition;

			right = yRot * right;

			vec3 pan = (right * camera.panning.x) + (camera.up * camera.panning.y);

			mat4 view = glm::lookAt(newPosition + pan, camera.direction + pan, camYAxis);

			renderer.setViewMatrix(view);
			renderer.setCameraPosition(newPosition);
		}
	}
}
