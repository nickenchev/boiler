#include "core/spriteframe.h"
#include "display/opengl/opengltexture.h"

using namespace Boiler;

SpriteFrame::SpriteFrame(const Texture &sourceTexture, Rect sourceRect, bool rotated,
						 bool trimmed, const glm::vec2 &)
    : sourceTexture(sourceTexture), sourceRect(sourceRect)
{
    this->rotated = rotated;
    this->trimmed = trimmed;
}

SpriteFrame::SpriteFrame(const Texture &sourceTexture) : sourceTexture(sourceTexture)
{
}
