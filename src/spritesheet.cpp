#include "spritesheet.h"

SpriteSheet::SpriteSheet(std::string imageFile, Size size, SDL_Texture *texture, std::map<std::string, SpriteSheetFrame> &frames) : size(size), frames(frames)
{
    this->imageFile = imageFile;
    this->texture = texture;
}
