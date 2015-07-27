#include "spritesheet.h"

SpriteSheet::SpriteSheet(std::string imageFile, Size size, GLuint texture, std::map<std::string, SpriteSheetFrame> &frames) : size(size), frames(frames)
{
    this->imageFile = imageFile;
    this->texture = texture;
}
