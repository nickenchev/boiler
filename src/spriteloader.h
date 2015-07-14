#ifndef SPRITELOADER_H
#define SPRITELOADER_H

#include <string>
#include <vector>

#include "component.h"
#include "spritesheet.h"

class SDL_Texture;

class SpriteLoader : Component
{
    std::vector<SDL_Texture*> textures;
    std::vector<SpriteSheet> spriteSheets;

public:
    SpriteLoader(Engine &engine);
    ~SpriteLoader();

    //sprite handling methods
    void loadSheet(std::string filename);
    const std::vector<SpriteSheet> &getSpriteSheets() const { return spriteSheets; }
};

#endif // SPRITELOADER_H
