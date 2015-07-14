#include "spritesheet.h"

SpriteSheet::SpriteSheet(std::string imageFile, SDL_Texture *texture)
{
    this->imageFile = imageFile;
    this->texture = texture;
}
