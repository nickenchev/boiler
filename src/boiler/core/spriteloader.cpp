#include <fstream>
#include <iostream>
#include <string>

#include "spriteloader.h"
#include "json/json.h"
#include "engine.h"
#include "renderer.h"

#define COMPONENT_NAME "SpriteLoader"

SpriteLoader::SpriteLoader(Engine &engine) : Component(std::string(COMPONENT_NAME), engine)
{
}

const std::shared_ptr<SpriteSheet> SpriteLoader::loadSheet(std::string filename) const
{
    log("Loading " + filename);

    //read the spritesheet manifest
    std::ifstream jsonFile(filename);
    std::shared_ptr<SpriteSheet> sheet;
    if (jsonFile.is_open())
    {
        Json::Value json;
        jsonFile >> json;
        jsonFile.close();

        const int numFrames = json["frames"].size();
        std::string imageFile = "data/" + json["meta"]["image"].asString();

        //read the sprite image name and load the texture
        auto texture = getEngine().getImageLoader().loadImage(imageFile);

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

            // calculate the opengl texture coords
            float texX = x / static_cast<float>(width);
            float texY = y / static_cast<float>(height);
            float texW = (x + w) / static_cast<float>(width);
            float texH = (y + h) / static_cast<float>(height);
            GLfloat texCoords[] =
            {
                texX, texH,
                texW, texY,
                texX, texY,

                texX, texH,
                texW, texH,
                texW, texY
            };

            // create a VBO to hold each frame's texture coords
            GLuint texCoordVbo;
            glGenBuffers(1, &texCoordVbo);
            glBindBuffer(GL_ARRAY_BUFFER, texCoordVbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_DYNAMIC_DRAW);

            if (glGetError() != GL_NO_ERROR)
            {
                error("Unable to create the texture coordinate VBO.");
            }

            Rect sourceRect(x, y, w, h);
            bool rotated = sprite["rotated"].asBool();
            bool trimmed = sprite["trimmed"].asBool();

            int pivotX = sprite["pivot"]["x"].asInt();
            int pivotY = sprite["pivot"]["y"].asInt();
            glm::vec2 pivot(pivotX, pivotY);
            frames.insert(std::pair<std::string, SpriteSheetFrame>(frameFilename, SpriteSheetFrame(texture, frameFilename, sourceRect, rotated, trimmed, pivot, texCoordVbo)));
            log("Loaded frame: " + frameFilename);
        }
        sheet = std::make_shared<SpriteSheet>(imageFile, Size(width, height), texture, frames);
        const Size &size = sheet->getSize();
        log("Loaded " + filename + "(" + std::to_string(size.getWidth()) + ", " + std::to_string(size.getHeight()) + ")");
    }
    else
    {
        error("Coudn't find sprite sheet: " + filename);
    }
    return sheet;
}

SpriteLoader::~SpriteLoader()
{
}
