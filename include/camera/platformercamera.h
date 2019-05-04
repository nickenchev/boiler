#ifndef PLATFORMERCAMERA_H
#define PLATFORMERCAMERA_H

#include "camera.h"

namespace Boiler
{

class PlatformerCamera : public Camera
{
public:
    using Camera::Camera;

    void update(glm::vec3 moveAmount) override;
    glm::mat4 getViewMatrix() const override;
};

}

#endif /* PLATFORMERCAMERA_H */
