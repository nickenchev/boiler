#ifndef OPENGLTEXTURE_H
#define OPENGLTEXTURE_H

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

#endif /* OPENGLTEXTURE_H */
