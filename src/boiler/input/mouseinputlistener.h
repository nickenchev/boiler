#ifndef MOUSEINPUTLISTENER_H
#define MOUSEINPUTLISTENER_H

#include "inputlistener.h"

struct MouseButtonEvent;

class MouseInputListener : public InputListener
{
    int x, y;

public:
    MouseInputListener() : InputListener() { x = 0; y = 0; }

    int getX() const { return x; }
    void setX(const int x) { this->x = x; }
    int getY() const { return y; }
    void setY(const int y) { this->y = y; }

    virtual void onMouseMove() = 0;
    virtual void onMouseButton(const MouseButtonEvent event) = 0;
};

#endif /* MOUSEINPUTLISTENER_H */
