#ifndef SPRITELOADER_H
#define SPRITELOADER_H

#include <string>
#include <memory>
#include <vector>

#include "logger.h"
#include "spritesheet.h"
#include "video/texture.h"

typedef unsigned int GLuint;

namespace Boiler
{

class SpriteLoader
{
	Logger logger;

public:
    SpriteLoader();
    ~SpriteLoader();

    //sprite handling methods
    const std::shared_ptr<const SpriteSheet> loadJsonArray(std::string filename) const;
    const std::shared_ptr<const SpriteSheet> loadTexture(std::shared_ptr<const Texture> texture) const;
};

}

#endif // SPRITELOADER_H
