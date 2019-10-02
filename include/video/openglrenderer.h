#ifndef OPENGLRENDERER_H
#define OPENGLRENDERER_H

#include <SDL.h>
#include <memory>
#include "video/opengl.h"
#include "renderer.h"
#include "renderdetails.h"

namespace Boiler
{

#define COMPONENT_NAME "OpenGL Renderer"
#define VERSION COMPONENT_NAME

class Entity;
class VertexData;
class GLSLShaderProgram;

class OpenGLRenderer : public Renderer
{
    bool useGLES;
    
    SDL_GLContext glContext;
    SDL_Window *win = nullptr;
    unsigned int fbo, rbo;
	RenderDetails renderDetails;
	std::unique_ptr<GLSLShaderProgram> program;

public:
    OpenGLRenderer(bool useGLES);
    ~OpenGLRenderer();

    void initialize(const Size &size) override;
    void resize(const Size &size) override;
    std::string getVersion() const override { return std::string(VERSION); }

    SDL_Window *getWindow() const { return win; }
	SDL_GLContext getGLContext() const { return glContext; }

    std::shared_ptr<const Texture> createTexture(const std::string filePath, const Size &textureSize, const void *pixelData) const override;
    void setActiveTexture(std::shared_ptr<const Texture> texture) const override;

    std::shared_ptr<const Model> loadModel(const VertexData &data) const override;

	void beginRender() override;
	void endRender() override;

	void render(const glm::mat4 modelMatrix, const std::shared_ptr<const Model> model,
				const std::shared_ptr<const Texture> sourceTexture, const TextureInfo *textureInfo,
				const glm::vec4 &colour) override;

    void showMessageBox(const std::string &title, const std::string &message) override;
};

}

#endif /* OPENGLRENDERER_H */
