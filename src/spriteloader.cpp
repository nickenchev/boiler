#include <fstream>
#include <iostream>
#include "spriteloader.h"
#include "json/json.h"

void SpriteLoader::loadSheet(std::string filename)
{
    std::ifstream jsonFile(filename);
    if (jsonFile.is_open())
    {
        Json::Value json;
        jsonFile >> json;
        for (auto it = json["frames"].begin(); it != json["frames"].end(); ++it)
        {
        std::cout << (*it)["filename"].asString() << std::endl;
        }
        jsonFile.close();
    }
    else
    {
        std::cerr << "Error loading sprite sheet: " << filename << std::endl;
    }
}
