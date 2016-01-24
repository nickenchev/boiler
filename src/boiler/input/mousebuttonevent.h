#ifndef MOUSEBUTTONEVENT_H
#define MOUSEBUTTONEVENT_H

#include "inputevent.h"

enum class MouseButton { LEFT, MIDDLE, RIGHT };

struct MouseButtonEvent : public InputEvent
{
    MouseButton button;
    ButtonState state;
    MouseButtonEvent(const MouseButton button, const ButtonState state) : button(button), state(state), InputEvent() { }
};


#endif /* MOUSEBUTTONEVENT_H */
