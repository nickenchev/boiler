#include "blankpart.h"

BlankPart::BlankPart()
{
}

void BlankPart::onCreate()
{
    //Engine::getInstance().addKeyListener(this);
    Engine::getInstance().getRenderer().setClearColor(Color3(0.8f, 0.8f, 1.0f));
}

void BlankPart::update()
{
}

void BlankPart::onKeyStateChanged(const KeyInputEvent &event)
{
    //if (event.keyCode == SDLK_ESCAPE)
    //{
    //Engine::getInstance().quit();
    //}
}
