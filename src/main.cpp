#include <iostream>
#include <string>
#include "spriteloader.h"
#include "engine.h"
#include "renderer.h"
#include "triptych/gamepart.h"

int main()
{
    std::cout << "* Starting..." << std::endl;

    Engine engine;
    engine.initialize();

    auto part = std::make_shared<GamePart>(engine);
    engine.start(part);

    return 0;
}
