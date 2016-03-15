#include <iostream>
#include <string>

#include "engine.h"
#include "openglrenderer.h"
#include "terrain/terrainpart.h"

int main()
{
    Engine::getInstance().initialize(std::make_unique<OpenGLRenderer>(), 1024, 768);

    auto part = std::make_shared<TerrainPart>();
    Engine::getInstance().start(part);

    return 0;
}
