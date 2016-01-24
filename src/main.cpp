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

    engine.start(std::make_shared<GamePart>(engine));

    return 0;
}
