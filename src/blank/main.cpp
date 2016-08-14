#include "blankpart.h"
#include <iostream>
#include <string>
#include <boiler.h>

int main(int argc, char *argv[])
{
    Engine::getInstance().initialize(std::make_unique<OpenGLRenderer>(), 1024, 768);

    auto part = std::make_shared<BlankPart>();
    Engine::getInstance().start(part);

    return 0;
}
