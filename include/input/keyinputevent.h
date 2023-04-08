#ifndef KEYINPUTEVENT_H
#define KEYINPUTEVENT_H

#include <stdint.h>
#include "inputevent.h"

namespace Boiler
{

    using Keycode = uint32_t;

struct KeyInputEvent : public InputEvent
{
    Keycode keyCode;
    ButtonState state;

	KeyInputEvent()
	{
		keyCode = -1;
		state = ButtonState::NONE;
	}

    KeyInputEvent(Keycode keycode, ButtonState state)
    {
        this->keyCode = keyCode;
        this->state = state;
    }
};

}

#endif /* KEYINPUTEVENT_H */
