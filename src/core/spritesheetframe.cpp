#include "core/spritesheetframe.h"
#include "video/opengltexture.h"

using namespace Boiler;

SpriteSheetFrame::SpriteSheetFrame(const std::shared_ptr<const Texture> sourceTexture, std::string filename, Rect sourceRect, bool rotated, bool trimmed, const glm::vec2 &pivot, unsigned int texCoordsVbo)
    : sourceTexture(sourceTexture), filename(filename), sourceRect(sourceRect), pivot(pivot)
{
    this->rotated = rotated;
    this->trimmed = trimmed;
    this->texCoordsVbo = texCoordsVbo;
}

SpriteSheetFrame::SpriteSheetFrame(const std::shared_ptr<const Texture> sourceTexture, unsigned int texCoordsVbo)
    : sourceTexture(sourceTexture)
{
    this->texCoordsVbo = texCoordsVbo;
}
