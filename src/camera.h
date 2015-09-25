#ifndef CAMERA_H
#define CAMERA_H

#include "rect.h"

class Camera
{
public:
    Camera(const Rect &frame);

    Rect frame;
};


#endif /* CAMERA_H */
