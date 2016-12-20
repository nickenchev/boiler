#include <string>
#include <iostream>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "opengl.h"
#include "openglrenderer.h"
#include "opengltexture.h"
#include "core/spritesheetframe.h"
#include "core/engine.h"
#include "core/part.h"
#include "camera/camera.h"
#include "openglmodel.h"
#include "vertexdata.h"

OpenGLRenderer::OpenGLRenderer(bool useGLES) : Renderer(std::string(COMPONENT_NAME))
{
    this->useGLES = useGLES;
}

void setupGLExtensions()

{
#ifdef __APPLE__
    // require "experimental" as not all OpenGL features are marked "standard"
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Error initializing GLEW" << std::endl;
    }
    // glewInit() queries extensions incorrectly, clearing errors here
    glGetError();
#endif
}

void checkOpenGLErrors()
{
    GLenum err = GL_NO_ERROR;
    while((err = glGetError()) != GL_NO_ERROR)
    {
        std::string errorString;
        switch (err)
        {
            case GL_INVALID_ENUM:
            {
                errorString = "GL_INVALID_ENUM";
                break;
            }
            case GL_INVALID_VALUE:
            {
                errorString = "GL_INVALID_VALUE";
                break;
            }
            case GL_INVALID_OPERATION:
            {
                errorString = "GL_INVALID_OPERATION";
                break;
            }
            case GL_OUT_OF_MEMORY:
            {
                errorString = "GL_OUT_OF_MEMORY";
                break;
            }
            case GL_INVALID_FRAMEBUFFER_OPERATION:
            {
                errorString = "GL_INVALID_FRAMEBUFFER_OPERATION";
                break;
            }
            default:
            {
                errorString = "Other";
                break;
            }
        }
        Engine::getInstance().getRenderer().showMessageBox("OpenGL Error", errorString);
    }
}

void OpenGLRenderer::initialize(const Size screenSize)
{
    bool success = false;

    if (SDL_Init(SDL_INIT_VIDEO) == 0)
    {
        win = SDL_CreateWindow("Boiler", 0, 0,
                               screenSize.getWidth(),
                               screenSize.getHeight(), SDL_WINDOW_OPENGL);
        if (win)
        {
            // setup opengl
            std::string shaderPath;
            if (useGLES)
            {
                shaderPath = "data/shaders/es3/";
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
            }
            else
            {
                shaderPath = "data/shaders/";
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
                SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
            }

            glContext = SDL_GL_CreateContext(win);
            if (glContext)
            {
                std::cout << " - Using Context: OpenGL " << glGetString(GL_VERSION) << std::endl; 
                setupGLExtensions();

                IMG_Init(IMG_INIT_PNG);

                // compile the default shader program
                program = std::make_unique<ShaderProgram>(shaderPath, "shader");
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
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, screenSize.getWidth(), screenSize.getHeight());
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

std::shared_ptr<const Texture> OpenGLRenderer::createTexture(const std::string filePath, const Size &textureSize, const void *pixelData) const
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

    return std::move(std::make_shared<OpenGLTexture>(filePath, texId));
}

void OpenGLRenderer::setActiveTexture(const std::shared_ptr<const Texture> texture) const
{
    auto tex = std::static_pointer_cast<const OpenGLTexture>(texture);
    // the sprite becomes TEXTURE0 in the shader
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex->getOpenGLTextureId());
}

std::shared_ptr<const Model> OpenGLRenderer::loadModel(const VertexData &data) const
{
    return std::make_shared<OpenGLModel>(data);
}

void OpenGLRenderer::render() const
{
    glClearColor(getClearColor().x, getClearColor().y, getClearColor().z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    const std::vector<std::shared_ptr<Entity>> &entities = Engine::getInstance().getPart()->getChildren();

    RenderDetails renderDetails;
    const ShaderProgram *program = getProgram();
    if (program)
    {
        // grab the uniform locations
        glUseProgram(program->getShaderProgram());
        renderDetails.mvpUniform = glGetUniformLocation(program->getShaderProgram(), "MVP");
        renderDetails.colorUniform = glGetUniformLocation(program->getShaderProgram(), "entityColor");
        renderDetails.usingTexUniform = glGetUniformLocation(program->getShaderProgram(), "usingTexture");
    }

    // prepare the matrices
    const Size screenSize = Engine::getInstance().getScreenSize();
    const GLfloat orthoW = screenSize.getWidth() /  getGlobalScale().x;
    const GLfloat orthoH = screenSize.getHeight() / getGlobalScale().y;
    renderDetails.viewProjection = glm::ortho(0.0f,
                                          static_cast<GLfloat>(orthoW),
                                          static_cast<GLfloat>(orthoH), 0.0f, -1.0f, 1.0f);
    if (this->camera)
    {
        renderDetails.camViewProjection = renderDetails.viewProjection * camera->getViewMatrix();
    }

    // draw the entities recursively
    renderEntities(entities, renderDetails);

    SDL_GL_SwapWindow(win);
    glUseProgram(0);
}

void OpenGLRenderer::renderEntities(const std::vector<std::shared_ptr<Entity>> &entities, const RenderDetails &renderDetails) const
{
    for (auto &entity : entities)
    {
        if (entity->model)
        {
            auto model = std::static_pointer_cast<const OpenGLModel>(entity->model);
            // set the vao for the current sprite
            glBindVertexArray(model->getVao());

            if (entity->spriteFrame)
            {
                // binds the current frames texture VBO and ensure it is linked to the current VAO
                glBindBuffer(GL_ARRAY_BUFFER, entity->spriteFrame->getTexCoordsVbo());
                glVertexAttribPointer(ATTRIB_ARRAY_TEXTURE, 2, GL_FLOAT, GL_FALSE, 0, 0);

                // set the current texture
                setActiveTexture(entity->spriteFrame->getSourceTexture());
                glUniform1i(renderDetails.usingTexUniform, 1);
            }
            else
            {
                glUniform1i(renderDetails.usingTexUniform, 0);
                glUniform4fv(renderDetails.colorUniform, 1, glm::value_ptr(entity->color));
            }

            const glm::mat4 &modelMatrix = entity->getMatrix();
            glm::mat4 mvpMatrix;

            // absolute entities aren't affected by the camera
            if (entity->absolute)
            {
                mvpMatrix = renderDetails.viewProjection * modelMatrix;
            }
            else
            {
                mvpMatrix = renderDetails.camViewProjection * modelMatrix;
            }

            glUniformMatrix4fv(renderDetails.mvpUniform, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

            // draw the entity
            glDrawArrays(GL_TRIANGLES, 0, model->getNumVertices());
            glBindVertexArray(0);

            GLenum glError = glGetError();
            if (glError != GL_NO_ERROR)
            {
                error("GL Error returned: " + std::to_string(glError));
            }
        }

        // draw the child entities
        if (entity->getChildren().size() > 0)
        {
            renderEntities(entity->getChildren(), renderDetails);
        }
    }
}

void OpenGLRenderer::showMessageBox(const std::string &title, const std::string &message)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, title.c_str(), message.c_str(), win);
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

