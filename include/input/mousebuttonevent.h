#ifndef MOUSEBUTTONEVENT_H
#define MOUSEBUTTONEVENT_H

#include "inputevent.h"

namespace Boiler
{

enum class MouseButton { LEFT, MIDDLE, RIGHT };

struct MouseButtonEvent : public InputEvent
{
    MouseButton button;
    ButtonState state;
    MouseButtonEvent(const MouseButton button, const ButtonState state) : InputEvent(), button(button), state(state) { }
};

}

#endif /* MOUSEBUTTONEVENT_H */
