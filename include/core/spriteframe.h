#ifndef SPRITEFRAME_H
#define SPRITEFRAME_H

#include <memory>
#include <string>
#include "rect.h"
#include "display/texture.h"
#include "display/textureinfo.h"

namespace Boiler
{

class SpriteFrame
{
    const Texture sourceTexture;
    Rect sourceRect;
    bool rotated;
    bool trimmed;

public:
	SpriteFrame(const Texture &sourceTexture, Rect sourceRect, bool rotated,
				bool trimmed, const glm::vec2 &pivot);
    SpriteFrame(const Texture &sourceTexture);

    Rect getSourceRect() const { return sourceRect; }
    bool isRotated() const { return rotated; }
    bool isTrimmed() const { return trimmed; }
    const Texture &getSourceTexture() const { return sourceTexture; }
};

}


#endif /* SPRITEFRAME_H */
