#include "spritesheetframe.h"


SpriteSheetFrame::SpriteSheetFrame(const std::shared_ptr<Texture> sourceTexture, std::string filename, Rect sourceRect, bool rotated, bool trimmed, const glm::vec2 &pivot, unsigned int texCoordsVbo)
    : sourceTexture(sourceTexture), filename(filename), sourceRect(sourceRect), pivot(pivot)
{
    this->rotated = rotated;
    this->trimmed = trimmed;
    this->texCoordsVbo = texCoordsVbo;
}
