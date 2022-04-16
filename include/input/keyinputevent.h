#ifndef KEYINPUTEVENT_H
#define KEYINPUTEVENT_H

#include <SDL2/SDL.h>
#include "inputevent.h"

namespace Boiler
{

struct KeyInputEvent : public InputEvent
{
    SDL_Keycode keyCode;
    ButtonState state;

    KeyInputEvent(SDL_Keycode keyCode, ButtonState state)
    {
        this->keyCode = keyCode;
        this->state = state;
    }
};

}

#endif /* KEYINPUTEVENT_H */
