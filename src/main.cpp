#include <iostream>
#include <string>
#include "spriteloader.h"
#include "engine.h"
#include "renderer.h"
#include "gamepart.h"

int main()
{
    std::cout << "* Starting..." << std::endl;
    Engine engine;
    engine.initialize();

    // custom init
    engine.getRenderer().setGlobalScale(glm::vec2(3.0f, 3.0f));

    GamePart part(engine);
    engine.start(&part);

    return 0;
}
