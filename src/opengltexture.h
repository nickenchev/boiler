#ifndef OPENGLTEXTURE_H
#define OPENGLTEXTURE_H

#include "texture.h"

struct Size;

class OpenGLTexture : public Texture
{
    unsigned int textureId;

public:
    OpenGLTexture(unsigned int textureId);
    
    unsigned int getOpenGLTextureId() const { return textureId; }
};


#endif /* OPENGLTEXTURE_H */
