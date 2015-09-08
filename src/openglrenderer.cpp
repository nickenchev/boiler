#define GLM_COMPILER 0
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "opengl.h"
#include "openglrenderer.h"
#include "opengltexture.h"
#include "spritesheetframe.h"
#include "engine.h"
#include "gamepart.h"

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

void OpenGLRenderer::render() const
{
    const ShaderProgram *program = getEngine().getProgram();
    glUseProgram(program->getShaderProgram());
    GLuint mvpUniform = glGetUniformLocation(program->getShaderProgram(), "MVP");

    const std::vector<std::shared_ptr<Entity>> &entities = getEngine().getPart()->getEntities();
    for (auto &entity : entities)
    {
        // set the vao for the current sprite
        glBindVertexArray(entity->getVao());

        // binds the current frames texture VBO and ensure it is linked to the current VAO
        glBindBuffer(GL_ARRAY_BUFFER, entity->spriteFrame->getTexCoordsVbo());
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);

        // opengl sprite render
        glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(getEngine().getScreenWidth()),
                                        static_cast<GLfloat>(getEngine().getScreenHeight()), 0.0f, -1.0f, 1.0f);

        const glm::mat4 &model = entity->getMatrix();

        // get the final matrix
        projection = projection * model;
        glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, &projection[0][0]);

        // set the current texture
        setActiveTexture(entity->spriteSheet->getTexture());

        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    glUseProgram(0);
}
