#include <string>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "opengl.h"
#include "openglrenderer.h"
#include "opengltexture.h"
#include "spritesheetframe.h"
#include "engine.h"
#include "part.h"
#include "camera.h"

#define COMPONENT_NAME "OpenGL Renderer"

OpenGLRenderer::OpenGLRenderer() : Renderer(std::string(COMPONENT_NAME))
{
    bool success = false;
    const Size screenSize = Engine::getInstance().getScreenSize();

    if (SDL_Init(SDL_INIT_VIDEO) == 0)
    {
        win = SDL_CreateWindow("Boiler", 0, 0,
                               screenSize.getWidth(),
                               screenSize.getHeight(), SDL_WINDOW_OPENGL);
        if (win)
        {
            // setup opengl
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

            glContext = SDL_GL_CreateContext(win);
            if (glContext)
            {
                std::cout << " - Using Context: OpenGL " << glGetString(GL_VERSION) << std::endl; 

                // require "experimental" as not all OpenGL features are marked "standard"
                glewExperimental = GL_TRUE;
                if (glewInit() != GLEW_OK)
                {
                    std::cerr << "Error initializing GLEW" << std::endl;
                }
                // glewInit() queries extensions incorrectly, clearing errors here
                glGetError();
                
                IMG_Init(IMG_INIT_PNG);

                // compile the default shader program
                program = std::make_unique<ShaderProgram>("shader");
                success = true;
            }
        }
    }

    if (!success)
    {
        std::cout << "Error Initializing SDL: " << SDL_GetError() << std::endl;
    }

    // setup a RBO for a colour target
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, screenSize.getWidth(), screenSize.getHeight());
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // setup the FBO
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo);

    // make sure FBO is all good
    GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
    {
        error("Error initializing FBO / RBO target.");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // enable blending on all buffers
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

std::shared_ptr<Texture> OpenGLRenderer::createTexture(const std::string filePath, const Size &textureSize, const void *pixelData) const
{
    GLuint texId;
    // create the opengl texture and fill it with surface data
    glGenTextures(1, &texId);

    glBindTexture(GL_TEXTURE_2D, texId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, textureSize.getWidth(), textureSize.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);

    // set nearest neighbour filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // unbind the texId 
    glBindTexture(GL_TEXTURE_2D, 0);

    if (glGetError() != GL_NO_ERROR)
    {
        error("Unable to create GL texId.");
    }
    else
    {
        log("Created texture with ID: " + std::to_string(texId));
    }

    return std::make_shared<OpenGLTexture>(filePath, texId);
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
    const ShaderProgram *program = Engine::getInstance().getRenderer().getProgram();
    glUseProgram(program->getShaderProgram());
    GLuint mvpUniform = glGetUniformLocation(program->getShaderProgram(), "MVP");

    glClearColor(getClearColor().x, getClearColor().y, getClearColor().z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    const std::vector<std::shared_ptr<Entity>> &entities = Engine::getInstance().getPart()->getChildren();

    // prepare the matrices
    const Size screenSize = Engine::getInstance().getScreenSize();
    const GLfloat orthoW = screenSize.getWidth() / Engine::getInstance().getRenderer().getGlobalScale().x;
    const GLfloat orthoH = screenSize.getHeight() / Engine::getInstance().getRenderer().getGlobalScale().y;

    // opengl sprite render
    glm::mat4 viewProjection = glm::ortho(0.0f, static_cast<GLfloat>(orthoW),
                                      static_cast<GLfloat>(orthoH), 0.0f, -1.0f, 1.0f);
    if (this->camera)
    {
        const Rect &rect = camera->frame;
        glm::mat4 view = glm::lookAt(glm::vec3(rect.position.x, rect.position.y, 1.0f),
                                        glm::vec3(rect.position.x, rect.position.y, -1.0f),
                                        glm::vec3(0, 1.0f, 0));
        viewProjection = viewProjection * view;
    }

    // draw the entities recursively
    renderEntities(entities, mvpUniform, viewProjection, glm::vec3(0, 0, 0));

    SDL_GL_SwapWindow(win);
    glUseProgram(0);
}

void OpenGLRenderer::renderEntities(const std::vector<std::shared_ptr<Entity>> &entities, unsigned int mvpUniform, const glm::mat4 &viewProjection, const glm::vec3 offset) const
{
    for (auto &entity : entities)
    {
        glm::vec3 entityOffset = entity->frame.position + offset;

        // set the vao for the current sprite
        glBindVertexArray(entity->getVao());

        if (entity->spriteFrame)
        {
            // binds the current frames texture VBO and ensure it is linked to the current VAO
            glBindBuffer(GL_ARRAY_BUFFER, entity->spriteFrame->getTexCoordsVbo());
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);
            // set the current texture
            setActiveTexture(entity->spriteSheet->getTexture());
        }

        const glm::mat4 &model = entity->getMatrix(offset);
        glm::mat4 mvpMatrix = viewProjection * model;

        glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, &mvpMatrix[0][0]);

        // draw the entity
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // draw the child entities
        if (entity->getChildren().size() > 0)
        {
            renderEntities(entity->getChildren(), mvpUniform, viewProjection, entityOffset);
        }
    }
}

OpenGLRenderer::~OpenGLRenderer()
{
    if (glContext)
    {
        std::cout << "* Destroying GL Context" << std::endl;
        SDL_GL_DeleteContext(glContext);
    }
    if (win)
    {
        std::cout << "* Destroying Window" << std::endl;
        SDL_DestroyWindow(win);
    }
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &fbo);
}
