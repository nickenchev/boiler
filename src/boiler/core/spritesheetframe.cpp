#include "spritesheetframe.h"


SpriteSheetFrame::SpriteSheetFrame(std::string filename, Rect sourceRect, bool rotated, bool trimmed, const glm::vec2 &pivot, unsigned int texCoordsVbo)
    : filename(filename), sourceRect(sourceRect), pivot(pivot)
{
    this->rotated = rotated;
    this->trimmed = trimmed;
    this->texCoordsVbo = texCoordsVbo;
}
