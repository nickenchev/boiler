#include "blankpart.h"

BlankPart::BlankPart()
{
}

void BlankPart::onCreate()
{
    Engine::getInstance().addKeyListener(this);
}

void BlankPart::update()
{
}

void BlankPart::onKeyStateChanged(const KeyInputEvent &event)
{
    if (event.keyCode == SDLK_ESCAPE)
    {
        Engine::getInstance().quit();
    }
}
