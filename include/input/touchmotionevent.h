#ifndef TOUCHMOTIONEVENT_H
#define TOUCHMOTIONEVENT_H

namespace Boiler
{

struct TouchMotionEvent
{
    float x, y, xDistance, yDistance;
    TouchMotionEvent(float x, float y, float xDistance, float yDistance)
    {
        this->x = x;
        this->y = y;
        this->xDistance = xDistance;
        this->yDistance = yDistance;
    }
};

}

#endif /* TOUCHMOTIONEVENT_H */
