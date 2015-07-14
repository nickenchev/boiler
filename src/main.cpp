#include <iostream>
#include <string>
#include "spriteloader.h"
#include "engine.h"

void load(Engine &engine);

int main()
{
    std::cout << "* Starting..." << std::endl;
    Engine engine;
    engine.initialize();

    load(engine);
    
    engine.start();
    engine.start();

    return 0;
}

void load(Engine &engine)
{
    std::string filename = "data/random.json";
    engine.getSpriteLoader().loadSheet(filename);
}
