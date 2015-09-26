#ifndef PLATFORMERCAMERA_H
#define PLATFORMERCAMERA_H

#include "camera.h"

class PlatformerCamera : public Camera
{
public:
    using Camera::Camera;

    void update(glm::vec2 moveAmount) override;
};


#endif /* PLATFORMERCAMERA_H */
