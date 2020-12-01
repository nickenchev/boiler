#ifndef MOUSEMOTIONEVENT_H
#define MOUSEMOTIONEVENT_H

namespace Boiler
{

struct MouseMotionEvent
{
    float x, y, xDistance, yDistance;

	MouseMotionEvent() : MouseMotionEvent(0, 0, 0, 0) {}
    MouseMotionEvent(float x, float y, float xDistance, float yDistance)
    {
        this->x = x;
        this->y = y;
        this->xDistance = xDistance;
        this->yDistance = yDistance;
    }
};

}

#endif /* MOUSEMOTIONEVENT_H */
