#ifndef SPRITELOADER_H
#define SPRITELOADER_H

#include <string>
#include <memory>
#include <vector>

#include "component.h"
#include "spritesheet.h"

class SDL_Texture;

class SpriteLoader : Component
{
    std::vector<SDL_Texture*> textures;
    std::vector<std::unique_ptr<SpriteSheet>> spriteSheets;

public:
    SpriteLoader(Engine &engine);
    ~SpriteLoader();

    //sprite handling methods
    SpriteSheet *loadSheet(std::string filename);
    const std::vector<std::unique_ptr<SpriteSheet>> &getSpriteSheets() const { return spriteSheets; }
};

#endif // SPRITELOADER_H
