#include <string>
#include <iostream>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "core/boiler.h"
#include "video/openglrenderer.h"
#include "video/opengltexture.h"
#include "video/openglmodel.h"
#include "video/vertexdata.h"
#include "core/spritesheetframe.h"
#include "core/boiler.h"
#include "core/part.h"
#include "core/components/positioncomponent.h"
#include "core/components/spritecomponent.h"
#include "camera/camera.h"

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
        Boiler::getInstance().getRenderer().showMessageBox("OpenGL Error", errorString);
    }
}

void OpenGLRenderer::initialize(const Size screenSize)
{
    bool success = false;

    if (SDL_Init(SDL_INIT_VIDEO) == 0)
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
			win = SDL_CreateWindow("Boiler", 0, 0,
								screenSize.width,
								screenSize.height, SDL_WINDOW_OPENGL);
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

                IMG_Init(IMG_INIT_PNG);

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



	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cout << "Could not initialize FreeType" << std::endl;
	}
	FT_Face face;
	if (FT_New_Face(ft, "data/fonts/8-Bit-Madness.ttf", 0, &face))
	{
		std::cout << "Could not load font" << std::endl;
	}
	FT_Set_Pixel_Sizes(face, 0, 48);

	// let opengl store textures that are not multiple of 4
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);   
	for (GLubyte c = 0; c < 128; ++c)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "Failed to load glyph" << std::endl;
		}
		else
		{
			GLuint texture = 0;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
			GLint swizzleMask[] = { GL_ONE, GL_ONE, GL_ONE, GL_RED };
			glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			Glyph glyph = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			glyphs.insert(std::pair<GLchar, Glyph>(c, glyph));
		}
	}
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
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
		//renderDetails.viewProjection = glm::ortho(0.0f, static_cast<GLfloat>(orthoW), static_cast<GLfloat>(orthoH), 0.0f, -1.0f, 1.0f);
		renderDetails.viewProjection = glm::ortho(0.0f, static_cast<GLfloat>(orthoW), 0.0f, static_cast<GLfloat>(orthoH));
    }

	// if camera has been set, recalc the projection matrix
    if (this->camera)
    {
        renderDetails.camViewProjection = renderDetails.viewProjection * camera->getViewMatrix();
    }

    glClearColor(getClearColor().x, getClearColor().y, getClearColor().z, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLRenderer::endRender()
{
    SDL_GL_SwapWindow(win);
    glUseProgram(0);
}

void OpenGLRenderer::render(const PositionComponent &position, const SpriteComponent &sprite) const
{
	glUseProgram(getProgram()->getShaderProgram());
	// render the entity
	if (sprite.model)
    {
        auto model = std::static_pointer_cast<const OpenGLModel>(sprite.model);
        // set the vao for the current sprite
        glBindVertexArray(model->getVao());

		if (sprite.spriteFrame)
		{
			// binds the current frames texture VBO and ensure it is linked to the current VAO
			glBindBuffer(GL_ARRAY_BUFFER, sprite.spriteFrame->getTexCoordsVbo());
			glVertexAttribPointer(ATTRIB_ARRAY_TEXTURE, 2, GL_FLOAT, GL_FALSE, 0, 0);

			// set the current texture
			setActiveTexture(sprite.spriteFrame->getSourceTexture());
		}
		glUniform4fv(renderDetails.colorUniform, 1, glm::value_ptr(sprite.color));

		const glm::mat4 &modelMatrix = position.getMatrix();
		glm::mat4 mvpMatrix;

		// absolute entities aren't affected by the camera
		if (position.absolute || !this->camera)
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
			logger.error("GL Error returned: " + std::to_string(glError));
		}
	}

	std::string test = "Amazing!?";
	GLuint vao = 0;
	GLuint vbo = 0;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);

	// draw some text
	//glUseProgram(textProgram->getShaderProgram());
	//glUniform3f(glGetUniformLocation(textProgram->getShaderProgram(), "entityColor"), 0, 0, 1.0f);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(vao);

	GLfloat x = 0;
	GLfloat y = 100;
	GLfloat scale = 1.0f;
	
	for (char c : test)
	{
		const Glyph &glyph = glyphs.at(c);
		GLfloat xpos = x + glyph.bearing.x * scale;
		GLfloat ypos = y - (glyph.size.y - glyph.bearing.y) * scale;

		GLfloat w = glyph.size.x * scale;
		GLfloat h = glyph.size.y * scale;
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }           
        };

		glBindTexture(GL_TEXTURE_2D, glyph.textureID);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		x += (glyph.advance >> 6) * scale;

		GLenum glError = glGetError();
		if (glError != GL_NO_ERROR)
		{
			logger.error("GL Error returned: " + std::to_string(glError));
		}
	}

	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

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
