#include <iostream>
#include <string>

#include "video/opengl.h"
#include "core/spriteloader.h"
#include "json/json.h"
#include "core/boiler.h"
#include "video/renderer.h"
#include "util/filemanager.h"
#include "util/texutil.h"

#define COMPONENT_NAME "Sprite Loader"

SpriteLoader::SpriteLoader() : logger(std::string(COMPONENT_NAME))
{
}

const std::shared_ptr<const SpriteSheet> SpriteLoader::loadJsonArray(std::string filename) const
{
    logger.log("Loading " + filename);

    //read the spritesheet manifest
    std::string jsonContent = FileManager::readTextFile(filename);

    std::shared_ptr<SpriteSheet> sheet;
    Json::Reader jsonReader;
    Json::Value json;
    bool isParsed = jsonReader.parse(jsonContent, json, false);
    if (isParsed)
    {
        std::string imageFile = "data/" + json["meta"]["image"].asString();

        //read the sprite image name and load the texture
        auto texture = Boiler::getInstance().getImageLoader().loadImage(imageFile);

		const Size texSize(json["meta"]["size"]["w"].asFloat(), json["meta"]["size"]["h"].asFloat());

        std::map<std::string, SpriteSheetFrame> frames;
        //setup the individual frames
        for (auto it = json["frames"].begin(); it != json["frames"].end(); ++it)
        {
            Json::Value sprite = *it;
            std::string frameFilename = sprite["filename"].asString();
			Rect sourceRect(sprite["frame"]["x"].asFloat(), sprite["frame"]["y"].asFloat(),
							sprite["frame"]["w"].asFloat(), sprite["frame"]["h"].asFloat());

			const std::vector<GLfloat> texCoords = TextureUtil::getTextureCoords(texSize, sourceRect);

            // create a VBO to hold each frame's texture coords
            GLuint texCoordVbo = 0;
            glGenBuffers(1, &texCoordVbo);
            glBindBuffer(GL_ARRAY_BUFFER, texCoordVbo);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * texCoords.size(), &texCoords[0], GL_DYNAMIC_DRAW);

            if (glGetError() != GL_NO_ERROR)
            {
                logger.error("Unable to create the texture coordinate VBO.");
            }

            bool rotated = sprite["rotated"].asBool();
            bool trimmed = sprite["trimmed"].asBool();

            int pivotX = sprite["pivot"]["x"].asInt();
            int pivotY = sprite["pivot"]["y"].asInt();
            glm::vec2 pivot(pivotX, pivotY);
            frames.insert(std::pair<std::string, SpriteSheetFrame>(frameFilename, SpriteSheetFrame(texture, frameFilename, sourceRect,
																								   rotated, trimmed, pivot, texCoordVbo)));
            logger.log("Loaded frame: " + frameFilename);
        }
        sheet = std::make_shared<SpriteSheet>(imageFile, texSize, texture, frames);
        const Size &size = sheet->getSize();
        logger.log("Loaded " + filename + "(" + std::to_string(size.width) + ", " + std::to_string(size.height) + ")");
		
    }
    else
    {
        logger.error("Coudn't find sprite sheet: " + filename);
    }
    return sheet;
}

const std::shared_ptr<const SpriteSheet> SpriteLoader::loadTexture(std::shared_ptr<const Texture> texture) const
{
    // calculate the opengl texture coords
    const float texX = 0;
    const float texY = 0;
    const float texW = 1.0f;
    const float texH = 1.0f;
    const GLfloat texCoords[] =
    {
        texX, texH,
        texW, texY,
        texX, texY,

        texX, texH,
        texW, texH,
        texW, texY
    };

    // create a VBO to hold each frame's texture coords
    GLuint texCoordVbo = 0;
    glGenBuffers(1, &texCoordVbo);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texCoords), texCoords, GL_DYNAMIC_DRAW);

    if (glGetError() != GL_NO_ERROR)
    {
        logger.error("Unable to create the texture coordinate VBO.");
    }
    std::map<std::string, SpriteSheetFrame> frames;
    frames.insert(std::pair<std::string, SpriteSheetFrame>("default", SpriteSheetFrame(texture, texCoordVbo)));

    return std::make_shared<const SpriteSheet>("procedural", Size(0, 0), texture, frames);
}

SpriteLoader::~SpriteLoader()
{
}
