#include <iostream>
#include "opengl.h"
#include "opengltexture.h"
#include "rect.h"

OpenGLTexture::OpenGLTexture(const std::string filePath, unsigned int textureId) : Texture(filePath)
{
    this->textureId = textureId;
}

OpenGLTexture::~OpenGLTexture()
{
    std::cout << "* Deleting OpenGL Texture: " << textureId << " - " << getFilePath() << std::endl;
    if (textureId)
    {
        glDeleteTextures(1, &textureId);
    }
}
