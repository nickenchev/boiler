#include <fstream>
#include <iostream>
#include <string>
#include <SDL_image.h>

#include "spriteloader.h"
#include "json/json.h"
#include "engine.h"

#define COMPONENT_NAME "SpriteLoader"

SpriteLoader::SpriteLoader(Engine &engine) : Component(std::string(COMPONENT_NAME), engine)
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
        
        std::string imageFile = "data/" + json["meta"]["image"].asString();

        //read the sprite image name and load the texture
        SDL_Renderer *renderer = getEngine().getRenderer();
        SDL_Surface *surface = IMG_Load(imageFile.c_str());
        SDL_Texture *texture = nullptr;
        if (surface)
        {
            texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (texture)
            {
                textures.push_back(texture);
                SDL_FreeSurface(surface);
            }
        }

        int width = json["meta"]["size"]["w"].asInt();
        int height = json["meta"]["size"]["h"].asInt();
        SpriteSheet sheet(imageFile, Size(width, height), texture);

        //setup the individual frames
        for (auto it = json["frames"].begin(); it != json["frames"].end(); ++it)
        {
            Json::Value frame = *it;
            std::string frameFilename = frame["filename"].asString();
        }
        jsonFile.close();
        spriteSheets.push_back(sheet);

        const Size &size = sheet.getSize();
        log("Loaded " + filename + "(" + std::to_string(size.width) + ", " + std::to_string(size.height) + ")");
    }
    else
    {
        error("Coudn't find sprite sheet: " + filename);
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
