#include <iostream>
#include <boiler.h>
#include <array2d.h>
#include "terrainpart.h"

TerrainPart::TerrainPart() : heightMap(512, 512)
{
    heightMap(1, 1) = 1.0f; // top-left
    heightMap(1, heightMap.getColumns()) = 1.0f; // top-right
    heightMap(heightMap.getRows(), heightMap.getColumns()) = 1.0f; // bottom-left
    heightMap(heightMap.getRows(), 0) = 1.0f; // bottom-right
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
