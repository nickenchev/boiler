#ifndef SPRITESHEET_H
#define SPRITESHEET_H

#include <memory>
#include <map>
#include <string>
#include "rect.h"
#include "spritesheetframe.h"

class SDL_Texture;

class SpriteSheet
{
    SDL_Texture *texture;
    std::string imageFile;
    Size size;
    int numFrames;
    std::map<std::string, SpriteSheetFrame> frames;

public:
    SpriteSheet(std::string imageFile, Size size, SDL_Texture *texture, int numFrames);

    SDL_Texture *getTexture() const { return texture; }
    std::string getImageFile() const { return imageFile; }
    const Size &getSize() const { return size; }
    void addFrame(const SpriteSheetFrame &frame);
};

#endif // SPRITESHEET_H
