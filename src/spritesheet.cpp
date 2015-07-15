#include "spritesheet.h"

SpriteSheet::SpriteSheet(std::string imageFile, Size size, SDL_Texture *texture, int numFrames) : size(size)
{
    this->imageFile = imageFile;
    this->texture = texture;
    this->numFrames = numFrames;
}

void SpriteSheet::addFrame(const SpriteSheetFrame &frame)
{
}
