#ifndef SPRITELOADER_H
#define SPRITELOADER_H

#include <string>
#include <memory>
#include <vector>

#include "component.h"
#include "spritesheet.h"
#include "texture.h"

class SDL_Texture;
typedef unsigned int GLuint;

class SpriteLoader : Component
{
public:
    SpriteLoader();
    ~SpriteLoader();

    //sprite handling methods
    const std::shared_ptr<const SpriteSheet> loadSheet(std::string filename) const;
    const std::shared_ptr<const SpriteSheet> loadSheet(std::shared_ptr<const Texture> texture) const;
};

#endif // SPRITELOADER_H
