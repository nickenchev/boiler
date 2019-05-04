#ifndef TOUCHTAPEVENT_H
#define TOUCHTAPEVENT_H

namespace Boiler
{

enum class TapState { UP, DOWN };

struct TouchTapEvent
{
    TapState state;
    TouchTapEvent(TapState state)
    {
        this->state = state;
    }
};

}

#endif /* TOUCHTAPEVENT_H */
