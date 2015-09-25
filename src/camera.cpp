#include "camera.h"

Camera::Camera(const Rect &frame) : frame(frame), centralEntity(nullptr)
{
}

void Camera::setCentralEntity(std::shared_ptr<Entity> centralEntity)
{
    this->centralEntity = centralEntity;
}
