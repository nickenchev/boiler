#include "terrainpart.h"
#include <iostream>
#include <string>
#include <boiler.h>

int main(int argc, char *argv[])
{
    Engine::getInstance().initialize(std::make_unique<OpenGLRenderer>(true), 1024, 768);

    auto part = std::make_shared<TerrainPart>();
    Engine::getInstance().start(part);

    return 0;
}
