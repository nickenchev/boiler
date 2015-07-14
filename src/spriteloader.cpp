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
void SpriteLoader::loadSheet(std::string filename)
{
    //read the spritesheet manifest
    std::ifstream jsonFile(filename);
    if (jsonFile.is_open())
    {
        Json::Value json;
        jsonFile >> json;
        
        std::string imageFile = json["meta"]["image"].asString();
        std::cout << " - Loading " << filename << "(" << imageFile << ")" << std::endl;

        //read the sprite image name and load the texture
        SDL_Renderer *renderer = getEngine().getRenderer();
        SDL_Surface *surface = IMG_Load(imageFile.c_str());
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        textures.push_back(texture);

        SpriteSheet sheet(imageFile, texture);

        //setup the individual frames
        for (auto it = json["frames"].begin(); it != json["frames"].end(); ++it)
        {
        }
        jsonFile.close();

        spriteSheets.push_back(sheet);
    }
    else
    {
        std::cerr << "Error loading sprite sheet: " << filename << std::endl;
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
