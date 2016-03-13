#ifndef KEYINPUTLISTENER_H
#define KEYINPUTLISTENER_H

#include "inputlistener.h"
#include "keyinputevent.h"

class KeyInputListener : public InputListener
{
public:
    KeyInputListener() : InputListener() { }

    virtual void onKeyStateChanged(const KeyInputEvent &event) = 0;
};



#endif /* KEYINPUTLISTENER_H */
