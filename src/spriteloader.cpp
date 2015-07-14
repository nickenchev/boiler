#include <fstream>
#include <iostream>
#include <string>
#include <SDL_image.h>

#include "spriteloader.h"
#include "json/json.h"
#include "engine.h"

SpriteLoader::SpriteLoader(Engine &engine) : Component(engine)
{
}
void SpriteLoader::loadSheet(std::string dataFile, std::string imageFile)
{
    //load the image texture
    SDL_Renderer *renderer = getEngine().getRenderer();
    SDL_Surface *surface = IMG_Load(imageFile.c_str());
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    textures.push_back(texture);

    SpriteSheet sheet(texture);
    std::ifstream jsonFile(dataFile);
    if (jsonFile.is_open())
    {
        Json::Value json;
        jsonFile >> json;
        for (auto it = json["frames"].begin(); it != json["frames"].end(); ++it)
        {
            std::cout << (*it)["filename"].asString() << std::endl;
        }
        jsonFile.close();

        spriteSheets.push_back(sheet);
    }
    else
    {
        std::cerr << "Error loading sprite sheet: " << dataFile << std::endl;
    }
}

SpriteLoader::~SpriteLoader()
{
    std::cout << "* SpriteLoader cleanup (" << textures.size() << " textures)." << std::endl;
    for (auto it = textures.begin(); it != textures.end(); ++it)
    {
        SDL_DestroyTexture(*it);
    }
}
