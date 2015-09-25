#ifndef PLATFORMERCAMERA_H
#define PLATFORMERCAMERA_H

#include "camera.h"

class PlatformerCamera : public Camera
{
public:
    PlatformerCamera(const Rect &frame) : Camera(frame) { }

    void update();
};


#endif /* PLATFORMERCAMERA_H */
