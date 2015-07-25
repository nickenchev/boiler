#include "spritesheetframe.h"

SpriteSheetFrame::SpriteSheetFrame(std::string filename, Rect sourceRect, bool rotated, bool trimmed, const glm::vec2 &pivot)
    : filename(filename), sourceRect(sourceRect), pivot(pivot)
{
    this->rotated = rotated;
    this->trimmed = trimmed;
}
