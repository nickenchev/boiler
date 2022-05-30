#include <glm/gtx/rotate_vector.hpp>

#include "display/renderer.h"
#include "core/componentstore.h"
#include "camera/camerasystem.h"
#include "core/components/transformcomponent.h"
#include "physics/movementcomponent.h"
#include "camera/cameracomponent.h"
#include "input/mousemotionevent.h"

using namespace Boiler;

CameraSystem::CameraSystem(Renderer &renderer) : System("Camera System"), renderer(renderer)
{
	expects<TransformComponent>();
	expects<CameraComponent>();
	expects<MovementComponent>();

	prevXFactor = 0;
	prevYFactor = 0;
}

void CameraSystem::update(AssetSet &assetSet, const FrameInfo &frameInfo, ComponentStore &store)
{
	for (Entity entity : getEntities())
	{
		TransformComponent &transform = store.retrieve<TransformComponent>(entity);
		CameraComponent &camera = store.retrieve<CameraComponent>(entity);
		MovementComponent &movement = store.retrieve<MovementComponent>(entity);

		movement.direction = glm::rotate(movement.direction, static_cast<float>(-movement.mouseXDiff), movement.up);
		const glm::vec3 axis = glm::cross(movement.up, movement.direction);
		movement.direction = glm::rotate(movement.direction, static_cast<float>(movement.mouseYDiff), axis);

		glm::mat4 view = glm::lookAt(transform.getPosition(), transform.getPosition() + movement.direction, movement.up);
		renderer.setViewMatrix(view);
		renderer.setCameraPosition(transform.getPosition());
	}
}
