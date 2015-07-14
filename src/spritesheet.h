#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <string>
#include "rect.h"

class SDL_Texture;

struct SpriteSheetFrame
{
    std::string filename;
};

class SpriteSheet
{
    SDL_Texture *texture;
    std::string imageFile;
    Size size;

public:
    SpriteSheet(std::string imageFile, Size size, SDL_Texture *texture);

    const SDL_Texture *getTexture() const { return texture; }
    std::string getImageFile() const { return imageFile; }
    const Size &getSize() const { return size; }
};

#endif // SPRITESHEET_H
