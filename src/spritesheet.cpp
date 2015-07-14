#include "spritesheet.h"

SpriteSheetFrame::SpriteSheetFrame(std::string filename, const SpriteSheet &spriteSheet, Rect sourceRect) : filename(filename), spriteSheet(spriteSheet), sourceRect(sourceRect)
{
}

SpriteSheet::SpriteSheet(std::string imageFile, Size size, SDL_Texture *texture) : size(size)
{
    this->imageFile = imageFile;
    this->texture = texture;
}
