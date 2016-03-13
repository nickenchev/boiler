#include <iostream>
#include <boiler.h>
#include "terrainpart.h"

TerrainPart::TerrainPart()
{
}

void TerrainPart::onCreate()
{
    Engine::getInstance().addKeyListener(this);
}

void TerrainPart::onKeyStateChanged(const KeyInputEvent &event)
{
    if (event.keyCode == SDLK_ESCAPE)
    {
        Engine::getInstance().quit();
    }
}
