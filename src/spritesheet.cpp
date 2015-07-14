#include "spritesheet.h"

SpriteSheet::SpriteSheet(std::string imageFile, Size size, SDL_Texture *texture) : size(size)
{
    this->imageFile = imageFile;
    this->texture = texture;
}
