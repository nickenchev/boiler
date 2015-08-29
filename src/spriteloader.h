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
    SpriteLoader(Engine &engine);
    ~SpriteLoader();

    //sprite handling methods
    std::shared_ptr<SpriteSheet> loadSheet(std::string filename);
};

#endif // SPRITELOADER_H
