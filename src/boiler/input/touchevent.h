#ifndef TOUCHEVENT_H
#define TOUCHEVENT_H

enum class TouchState { UP, DOWN };

struct TouchEvent
{
    TouchState state;
    TouchEvent(TouchState state)
    {
        this->state = state;
    }
};

#endif /* TOUCHEVENT_H */
