#include "terrainpart.h"
#include <iostream>
#include <string>
#include <boiler.h>

int main(int argc, char *argv[])
{
    Engine::getInstance().initialize(std::make_unique<OpenGLRenderer>(false), 2560, 1440);

    auto part = std::make_shared<TerrainPart>();
    Engine::getInstance().start(part);

    return 0;
}
