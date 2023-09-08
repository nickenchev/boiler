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

	prevXPos = 0;
	prevYPos = 0;
	prevXFactor = 0;
	prevYFactor = 0;
}

void CameraSystem::update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, EntityComponentSystem &ecs)
{
	for (Entity entity : getEntities())
	{
        TransformComponent &transform = ecs.retrieve<TransformComponent>(entity);
        CameraComponent &camera = ecs.retrieve<CameraComponent>(entity);
        MovementComponent &movement = ecs.retrieve<MovementComponent>(entity);
		PhysicsComponent& physics = ecs.retrieve<PhysicsComponent>(entity);

		transform.translation += physics.velocity;
		camera.direction = glm::rotate(camera.direction, static_cast<float>(-movement.mouseXDiff), camera.up);
		const glm::vec3 axis = glm::cross(camera.up, camera.direction);
		camera.direction = glm::rotate(camera.direction, static_cast<float>(movement.mouseYDiff), axis);

		// TODO: This shouldn't be happening directly on the renderer
		glm::mat4 view = glm::lookAt(transform.translation, transform.translation + camera.direction, camera.up);
		renderer.setViewMatrix(view);
		renderer.setCameraPosition(transform.translation);

		prevXPos = frameInfo.mouseXPos;
		prevYPos = frameInfo.mouseYPos;
	}
}
