#ifndef PANCAMERA_H
#define PANCAMERA_H

#include "camera.h"

class PanCemera : public Camera
{
public:
    using Camera::Camera;

    void update(glm::vec3 moveAmount) override;
    glm::mat4 getViewMatrix() const override;
};



#endif /* PANCAMERA_H */
