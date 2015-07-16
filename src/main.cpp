#include <iostream>
#include <string>
#include "spriteloader.h"
#include "engine.h"

int main()
{
    std::cout << "* Starting..." << std::endl;
    Engine engine;
    engine.initialize();
    engine.start();


    return 0;
}
