#include "camera/camera.h"

Camera::Camera(const Rect &frame, const Size &mapSize) : frame(frame), mapSize(mapSize), centralEntity(nullptr)
{
}

void Camera::setCentralEntity(std::shared_ptr<Entity> centralEntity)
{
    this->centralEntity = centralEntity;
}
