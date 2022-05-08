#include <glm/gtx/rotate_vector.hpp>

#include "display/renderer.h"
#include "core/componentstore.h"
#include "camera/camerasystem.h"
#include "core/components/transformcomponent.h"
#include "camera/cameracomponent.h"
#include "input/mousemotionevent.h"

using namespace Boiler;

CameraSystem::CameraSystem(Renderer &renderer) : System("Camera System"), renderer(renderer)
{
	expects<TransformComponent>();
	expects<CameraComponent>();

	prevXFactor = 0;
	prevYFactor = 0;

	/*
	auto mouseListener = [this, &renderer](const MouseMotionEvent &event)
	{
		// calculate mouse move factor based on current resolution
		const Size size = renderer.getScreenSize();
		const float xFactorNew = event.xDistance / size.width;
		const float yFactorNew = event.yDistance / size.height;

		const float alpha = 0.5f;
		const float xFactor = prevXFactor + alpha * (xFactorNew - prevXFactor);
		const float yFactor = prevYFactor + alpha * (yFactorNew - prevYFactor);
		//const float xFactor = xFactorNew;
		//const float yFactor = yFactorNew;

		const float sensitivity = 1.5;
		const float xDiff = sensitivity * xFactor;
		const float yDiff = sensitivity * yFactor;

		camDirection = glm::rotate(camDirection, static_cast<float>(-xDiff), camUp);
		const glm::vec3 axis = glm::cross(camUp, camDirection);
		camDirection = glm::rotate(camDirection, static_cast<float>(yDiff), axis);

		prevXFactor = xFactorNew;
		prevYFactor = yFactorNew;
	};

	engine.addMouseMotionListener(mouseListener);
	*/
}

void CameraSystem::update(const FrameInfo &frameInfo, ComponentStore &store)
{
	const Size size = renderer.getScreenSize();
	const float xFactorNew = frameInfo.mouseXDistance / size.width;
	const float yFactorNew = frameInfo.mouseYDistance / size.height;

	const float alpha = 0.5f;
	const float xFactor = prevXFactor + alpha * (xFactorNew - prevXFactor);
	const float yFactor = prevYFactor + alpha * (yFactorNew - prevYFactor);
	//const float xFactor = xFactorNew;
	//const float yFactor = yFactorNew;

	const float sensitivity = 1.5;
	const float xDiff = sensitivity * xFactor;
	const float yDiff = sensitivity * yFactor;

	for (Entity entity : getEntities())
	{
		TransformComponent &transform = store.retrieve<TransformComponent>(entity);
		CameraComponent &camera = store.retrieve<CameraComponent>(entity);

		camera.direction = glm::rotate(camera.direction, static_cast<float>(-xDiff), camera.up);
		const glm::vec3 axis = glm::cross(camera.up, camera.direction);
		camera.direction = glm::rotate(camera.direction, static_cast<float>(yDiff), axis);

		glm::mat4 view = glm::lookAt(transform.getPosition(), transform.getPosition() + camera.direction, camera.up);
		renderer.setViewMatrix(view);
		renderer.setCameraPosition(transform.getPosition());
	}

	prevXFactor = xFactorNew;
	prevYFactor = yFactorNew;
}
