#pragma once

#include <core/asset.h>

namespace Boiler
{
struct Size;

class OpenGLTexture
{
    unsigned int texture;

public:
    OpenGLTexture() = default;
    OpenGLTexture(unsigned int texture);
    
    unsigned int getOpenGLTextureId() const { return texture; }
};

}
