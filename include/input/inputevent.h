#ifndef INPUTEVENT_H
#define INPUTEVENT_H

#include <SDL2/SDL.h>

namespace Boiler
{

enum class ButtonState { UP, DOWN };

struct InputEvent
{
    SDL_Keycode keyCode;
    ButtonState state;

	InputEvent() {}

    InputEvent(SDL_Keycode keyCode, ButtonState state)
    {
        this->keyCode = keyCode;
        this->state = state;
    }
};

}

#endif /* INPUTEVENT_H */
