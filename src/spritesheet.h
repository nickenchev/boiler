#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <string>
#include "rect.h"

class SDL_Texture;

struct SpriteSheetFrame
{
    std::string filename;
    Rect frame;
    bool rotate;
    bool trimmed;
};

class SpriteSheet
{
    SDL_Texture *texture;
    std::string imageFile;

public:
    SpriteSheet(std::string imageFile, SDL_Texture *texture);

    const SDL_Texture *getTexture() const { return texture; }
    std::string getImageFile() const { return imageFile; }
};

#endif // SPRITESHEET_H
