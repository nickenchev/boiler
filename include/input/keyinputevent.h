#pragma once

#include <stdint.h>
#include "inputevent.h"
#include "input/keys.h"

namespace Boiler
{

struct KeyInputEvent : public InputEvent
{
    uint32_t keyCode;
    ButtonState state;

	KeyInputEvent()
	{
        keyCode = 0;
		state = ButtonState::NONE;
	}

    KeyInputEvent(uint32_t keyCode, ButtonState state)
    {
        this->keyCode = keyCode;
        this->state = state;
    }
};

}
