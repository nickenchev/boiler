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

SpriteSheet *SpriteLoader::loadSheet(std::string filename)
{
    //read the spritesheet manifest
    std::ifstream jsonFile(filename);
    if (jsonFile.is_open())
    {
        Json::Value json;
        jsonFile >> json;
        jsonFile.close();

        const int numFrames = json["frames"].size();
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

        std::map<std::string, SpriteSheetFrame> frames;
        //setup the individual frames
        for (auto it = json["frames"].begin(); it != json["frames"].end(); ++it)
        {
            Json::Value sprite = *it;
            std::string frameFilename = sprite["filename"].asString();
            int x = sprite["frame"]["x"].asInt();
            int y = sprite["frame"]["y"].asInt();
            int w = sprite["frame"]["w"].asInt();
            int h = sprite["frame"]["h"].asInt();
            Rect sourceRect(x, y, w, h);
            bool rotated = sprite["rotated"].asBool();
            bool trimmed = sprite["trimmed"].asBool();

            int pivotX = sprite["pivot"]["x"].asInt();
            int pivotY = sprite["pivot"]["y"].asInt();
            glm::vec2 pivot(pivotX, pivotY);
            frames.insert(std::pair<std::string, SpriteSheetFrame>(frameFilename,
                                                                   SpriteSheetFrame(frameFilename, sourceRect, rotated,
                                                                                    trimmed, pivot)));
        }
        auto sheet = std::make_unique<SpriteSheet>(imageFile, Size(width, height), texture, frames);
        const Size &size = sheet->getSize();
        log("Loaded " + filename + "(" + std::to_string(size.width) + ", " + std::to_string(size.height) + ")");

        //finally move the sheet into storage
        spriteSheets.push_back(std::move(sheet));
    }
    else
    {
        error("Coudn't find sprite sheet: " + filename);
    }
    return spriteSheets[spriteSheets.size() - 1].get();
}

SpriteLoader::~SpriteLoader()
{
    std::cout << "* SpriteLoader cleanup (" << textures.size() << " textures)." << std::endl;
    for (auto it = textures.begin(); it != textures.end(); ++it)
    {
        SDL_DestroyTexture(*it);
    }
}
