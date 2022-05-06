#include "camera/camerasystem.h"
#include "core/components/transformcomponent.h"

using namespace Boiler;

CameraSystem::CameraSystem() : System("Camera System")
{
	expects<TransformComponent>();
}

void CameraSystem::update(FrameInfo frameInfo, ComponentStore &store)
{
}
