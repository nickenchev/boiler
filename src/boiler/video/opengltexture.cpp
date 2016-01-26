#include <iostream>
#include "opengl.h"
#include "opengltexture.h"
#include "rect.h"

OpenGLTexture::OpenGLTexture(unsigned int textureId)
{
    this->textureId = textureId;
}

OpenGLTexture::~OpenGLTexture()
{
    std::cout << "* Deleting OpenGL Texture: " << textureId << std::endl;
    if (textureId)
    {
        glDeleteTextures(1, &textureId);
    }
}
