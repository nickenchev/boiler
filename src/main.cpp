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

    return 0;
}

void load(Engine &engine)
{
    std::string dataFile = "data/random.json";
    std::string imageFile = "data/random.png";

    engine.getSpriteLoader().loadSheet(dataFile, imageFile);
}
