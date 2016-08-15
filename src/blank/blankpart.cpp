#include "blankpart.h"

BlankPart::BlankPart()
{
}

void BlankPart::onCreate()
{
    Engine::getInstance().getRenderer().setClearColor(Color3(0.8f, 0.8f, 1.0f));
    Engine::getInstance().addKeyListener(this);
    auto test = std::make_shared<Entity>(Rect(10, 10, 50, 50));
    addChild(test);
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
