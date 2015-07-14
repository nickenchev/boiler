#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <string>
#include "rect.h"

class SDL_Texture;
class SpriteSheet;

class SpriteSheetFrame
{
    std::string filename;
    const SpriteSheet &spriteSheet;

public:
    SpriteSheetFrame(std::string filename, const SpriteSheet &spriteSheet) : filename(filename), spriteSheet(spriteSheet) { }
    const SpriteSheet &getSpriteSheet() const { return spriteSheet; }
};

class SpriteSheet
{
    SDL_Texture *texture;
    std::string imageFile;
    Size size;

public:
    SpriteSheet(std::string imageFile, Size size, SDL_Texture *texture);

    SDL_Texture *getTexture() const { return texture; }
    std::string getImageFile() const { return imageFile; }
    const Size &getSize() const { return size; }
};

#endif // SPRITESHEET_H
