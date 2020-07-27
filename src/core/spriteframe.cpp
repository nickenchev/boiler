#include "core/spriteframe.h"
#include "video/opengltexture.h"

using namespace Boiler;

SpriteFrame::SpriteFrame(const Texture &sourceTexture, Rect sourceRect, bool rotated,
						 bool trimmed, const glm::vec2 &pivot)
    : sourceTexture(sourceTexture), sourceRect(sourceRect), pivot(pivot)
{
    this->rotated = rotated;
    this->trimmed = trimmed;
}

SpriteFrame::SpriteFrame(const Texture &sourceTexture) : sourceTexture(sourceTexture)
{
}
