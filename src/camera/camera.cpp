#include "camera/camera.h"

using namespace Boiler;

Camera::Camera(const Rect &frame, const Size &mapSize) : centralEntity(nullptr), frame(frame), mapSize(mapSize) 
{
}

void Camera::setCentralEntity(std::shared_ptr<Entity> centralEntity)
{
    this->centralEntity = centralEntity;
}
