#include <iostream>
#include "video/opengl.h"
#include "video/opengltexture.h"
#include "core/rect.h"

OpenGLTexture::OpenGLTexture(const std::string filePath, unsigned int textureId) : Texture(filePath)
{
    this->textureId = textureId;
}

OpenGLTexture::~OpenGLTexture()
{
    std::cout << "* Deleting OpenGL Texture: " << textureId << " (" << getFilePath() << ")" << std::endl;
    if (textureId)
    {
        glDeleteTextures(1, &textureId);
    }
}
