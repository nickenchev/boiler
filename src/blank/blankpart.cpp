#include "blankpart.h"

const int mapWidth = 1024;
const int mapHeight = 1024;

BlankPart::BlankPart()
{
}

void BlankPart::onCreate()
{
    Engine::getInstance().getRenderer().setClearColor(Color3(0.8f, 0.8f, 1.0f));
    Engine::getInstance().addKeyListener(this);

    auto map = std::make_shared<Entity>(Rect(0, 0, mapWidth, mapHeight));
    addChild(map);
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
