#include "opengl.h"
#include "openglrenderer.h"
#include "opengltexture.h"

#define COMPONENT_NAME "OpenGL Renderer"

OpenGLRenderer::OpenGLRenderer(Engine &engine) : Renderer(std::string(COMPONENT_NAME), engine)
{
}

std::shared_ptr<Texture> OpenGLRenderer::createTexture(const Size &textureSize, const void *pixelData) const
{
    GLuint texId;
    // create the opengl texture and fill it with surface data
    glGenTextures(1, &texId);

    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureSize.width, textureSize.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);

    // set nearest neighbour filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // unbind the texId 
    glBindTexture(GL_TEXTURE_2D, 0);

    if (glGetError() != GL_NO_ERROR)
    {
        error("Unable to create GL texId.");
    }

    return std::make_shared<OpenGLTexture>(texId);
}

void OpenGLRenderer::setActiveTexture(const Texture &texture) const
{
    const OpenGLTexture &tex = (OpenGLTexture &)texture;
    // the sprite becomes TEXTURE0 in the shader
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex.getOpenGLTextureId());
}
