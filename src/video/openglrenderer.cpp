#include <string>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "core/engine.h"
#include "video/openglrenderer.h"
#include "video/opengltexture.h"
#include "video/openglmodel.h"
#include "video/vertexdata.h"
#include "video/opengltextureinfo.h"
#include "core/spritesheetframe.h"
#include "core/part.h"
#include "core/components/positioncomponent.h"
#include "core/components/spritecomponent.h"
#include "core/components/textcomponent.h"
#include "camera/camera.h"

using namespace Boiler;

OpenGLRenderer::OpenGLRenderer(bool useGLES) : Renderer(std::string(COMPONENT_NAME))
{
    this->useGLES = useGLES;
}

void setupGLExtensions()
{
#ifdef __APPLE__
    // require "experimental" as not all OpenGL features are marked "standard"
    glewExperimental = GL_TRUE;
#endif

	GLenum initStatus = glewInit();
    if (initStatus != GLEW_OK)
    {
		const GLubyte *errString = glewGetErrorString(initStatus);
        std::cerr << "GLEW Init Error: " << errString << std::endl;
    }
    // glewInit() queries extensions incorrectly, clearing errors here
    glGetError();
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

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) == 0)
    {
		if (useGLES)
		{
			SDL_DisplayMode displayMode;
			SDL_GetCurrentDisplayMode(0, &displayMode);
			
			win = SDL_CreateWindow("Boiler", 0, 0, displayMode.w, displayMode.h, SDL_WINDOW_OPENGL);
			setScreenSize(Size(displayMode.w, displayMode.h));
		}
		else
		{
			SDL_WindowFlags winFlags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
			win = SDL_CreateWindow("Boiler", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenSize.width, screenSize.height, winFlags);
			setScreenSize(screenSize);
		}

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
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
                SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
            }

            glContext = SDL_GL_CreateContext(win);
            if (glContext)
            {
                logger.log("Using Context: OpenGL " + std::string((const char *)glGetString(GL_VERSION)));
                setupGLExtensions();

				try
				{
					// compile the default shader program
					program = std::make_unique<ShaderProgram>(shaderPath, "basic.vert", "basic.frag");
					success = true;
				}
				catch (int exception)
				{
					showMessageBox("Error", "Error initializing Boiler");
				}
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
    glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, getScreenSize().width, getScreenSize().height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // setup the FBO
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rbo);

    // make sure FBO is all good
    GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
    {
        logger.error("Error initializing FBO / RBO target.");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // enable blending on all buffers
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// depth testing
	glEnable(GL_DEPTH_TEST);
}

void OpenGLRenderer::shutdown()
{
	SDL_VideoQuit();
}

std::shared_ptr<const Texture> OpenGLRenderer::createTexture(const std::string filePath, const Size &textureSize, const void *pixelData) const
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
        logger.error("Unable to create GL texId.");
    }
    else
    {
        logger.log("Created texture with ID: " + std::to_string(texId));
    }

    return std::make_shared<OpenGLTexture>(filePath, texId);
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

void OpenGLRenderer::beginRender()
{
    const ShaderProgram *program = getProgram();
    if (program)
    {
		renderDetails.shaderProgram = program;
        // grab the uniform locations
        glUseProgram(program->getShaderProgram());
        renderDetails.mvpUniform = glGetUniformLocation(program->getShaderProgram(), "MVP");
        renderDetails.colorUniform = glGetUniformLocation(program->getShaderProgram(), "entityColor");
		
		// prepare the matrices
		const Size screenSize = getScreenSize();
		const GLfloat orthoW = screenSize.width /  getGlobalScale().x;
		const GLfloat orthoH = screenSize.height / getGlobalScale().y;

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), screenSize.width / screenSize.height, 0.1f, 100.0f);
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::translate(view, glm::vec3(0, 0, -3.0f));
		renderDetails.viewProjection = projection * view;
		//renderDetails.viewProjection = glm::ortho(0.0f, static_cast<GLfloat>(orthoW), static_cast<GLfloat>(orthoH), 0.0f);
		//renderDetails.viewProjection = glm::ortho(0.0f, static_cast<GLfloat>(orthoW), static_cast<GLfloat>(orthoH), 0.0f, -1.0f, 1.0f);
    }

	const GLfloat color[] = { getClearColor().r, getClearColor().g, getClearColor().b, 1.0f};
	const GLfloat depth[] = { 0, 0, 0, 0 };
	glClearBufferfv(GL_COLOR, 0, color);
	//glClearBufferfv(GL_DEPTH, 0, depth);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderer::endRender()
{
    SDL_GL_SwapWindow(win);
    glUseProgram(0);
}

void OpenGLRenderer::render(const glm::mat4 modelMatrix, const std::shared_ptr<const Model> model,
							const std::shared_ptr<const Texture> sourceTexture, const TextureInfo *textureInfo,
							const glm::vec4 &colour) const
{
	glUseProgram(getProgram()->getShaderProgram());
	if (model)
    {
        auto oglModel = std::static_pointer_cast<const OpenGLModel>(model);

        // set the vao for the current sprite
        glBindVertexArray(oglModel->getVao());

		if (textureInfo)
		{
			auto *oglTexInfo = static_cast<const OpenGLTextureInfo *>(textureInfo);

			// binds the current frames texture VBO and ensure it is linked to the current VAO
			glBindBuffer(GL_ARRAY_BUFFER, oglTexInfo->getTexCoordsVbo());
            glVertexAttribPointer((GLuint)AttribArray::Texture, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

			// set the current texture
			setActiveTexture(sourceTexture);
		}
		glUniform4fv(renderDetails.colorUniform, 1, glm::value_ptr(colour));

		//const glm::mat4 mvpMatrix = renderDetails.viewProjection * modelMatrix;
		const glm::mat4 mvpMatrix = renderDetails.viewProjection * modelMatrix;
		glUniformMatrix4fv(renderDetails.mvpUniform, 1, GL_FALSE, glm::value_ptr(mvpMatrix));

		// draw the entity
		glDrawArrays(GL_TRIANGLES, 0, model->getNumVertices());
		glBindVertexArray(0);

		GLenum glError = glGetError();
		if (glError != GL_NO_ERROR)
		{
			logger.error("GL Error returned: " + std::to_string(glError));
		}
	}
}

/*
void OpenGLRenderer::render(const PositionComponent &position, const SpriteComponent &sprite) const
{
	render(position.toAbsolute(), sprite.model, sprite.spriteFrame->getSourceTexture(),
		   sprite.spriteFrame->getTexCoordsVbo(), sprite.colour);
}

void OpenGLRenderer::render(const PositionComponent &position, const TextComponent &text) const
{
	const GlyphMap &glyphMap = *text.glyphMap;

	float xOffset = 0;
    for (char character : text.text)
	{
        const Glyph &glyph = glyphMap[static_cast<unsigned long>(character)];

		PositionComponent glyphPos = position.toAbsolute();
		glyphPos.frame.position.x += xOffset + glyph.getBearing().x;
		glyphPos.frame.position.y += -glyph.getBearing().y;

		render(glyphPos, glyph.getModel(), glyphMap.getSourceTexture(), glyph.getTexCoordsVbo(), text.colour);

		// move to the next character position
		xOffset += (glyph.getAdvance() >> 6);
	}
}
*/

void OpenGLRenderer::showMessageBox(const std::string &title, const std::string &message)
{
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, title.c_str(), message.c_str(), win);
}

OpenGLRenderer::~OpenGLRenderer()
{
    if (glContext)
    {
		logger.log("Destroying GL Context");
        SDL_GL_DeleteContext(glContext);
    }
    if (win)
    {
		logger.log("Destroing Window");
        SDL_DestroyWindow(win);
    }
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &fbo);
}
