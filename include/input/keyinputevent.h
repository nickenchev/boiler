#ifndef KEYINPUTEVENT_H
#define KEYINPUTEVENT_H

#include <stdint.h>
#include "inputevent.h"
#include "input/keys.h"

namespace Boiler
{

struct KeyInputEvent : public InputEvent
{
    //KeyCode keyCode;
    uint32_t keyCode;
    ButtonState state;

	KeyInputEvent()
	{
		state = ButtonState::NONE;
	}

    KeyInputEvent(uint32_t keyCode, ButtonState state)
    {
        this->keyCode = keyCode;
        this->state = state;
    }
};

}

#endif /* KEYINPUTEVENT_H */
