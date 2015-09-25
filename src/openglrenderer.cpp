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
#include "gamepart.h"

#define COMPONENT_NAME "OpenGL Renderer"

OpenGLRenderer::OpenGLRenderer(Engine &engine) : Renderer(std::string(COMPONENT_NAME), engine)
{
    bool success = false;
    if (SDL_Init(SDL_INIT_VIDEO) == 0)
    {
        win = SDL_CreateWindow("Boiler", 0, 0, RES_WIDTH, RES_HEIGHT, SDL_WINDOW_OPENGL);
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
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA, getEngine().getScreenWidth(),
                          getEngine().getScreenHeight());
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
    const ShaderProgram *program = getEngine().getRenderer().getProgram();
    glUseProgram(program->getShaderProgram());
    GLuint mvpUniform = glGetUniformLocation(program->getShaderProgram(), "MVP");

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

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

        glm::mat4 model = entity->getMatrix();

        // factor in the camera position
        if (this->camera)
        {
            const Rect &rect = camera->frame;
            // generate a translation matrix to offset camera coords
            model = glm::translate(model, glm::vec3(-rect.position.x, -rect.position.y, 0));
        }
        
        // get the final matrix
        projection = projection * model;

        glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, &projection[0][0]);

        // set the current texture
        setActiveTexture(entity->spriteSheet->getTexture());

        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
    }

    SDL_GL_SwapWindow(win);
    glUseProgram(0);
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
