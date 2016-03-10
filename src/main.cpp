#include <iostream>
#include <string>
#include "spriteloader.h"
#include "engine.h"
#include "openglrenderer.h"
#include "triptych/gamepart.h"

int main()
{
    std::cout << "* Starting..." << std::endl;

    Engine::getInstance().initialize(std::make_unique<OpenGLRenderer>(), 640, 960);

    auto part = std::make_shared<GamePart>();
    Engine::getInstance().start(part);

    return 0;
}
