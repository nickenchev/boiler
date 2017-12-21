#ifndef OPENGLRENDERER_H
#define OPENGLRENDERER_H

#include <SDL.h>
#include <memory>
#include "renderer.h"

#define COMPONENT_NAME "OpenGL Renderer"
#define VERSION COMPONENT_NAME

class Entity;
class VertexData;

struct RenderDetails
{
    unsigned int mvpUniform;
    unsigned int colorUniform;
    bool usingTexUniform;
    glm::mat4 viewProjection;
    glm::mat4 camViewProjection;

    RenderDetails()
    {
        mvpUniform = -1;
        colorUniform = -1;
        usingTexUniform = -1;
    }
};

class OpenGLRenderer : public Renderer
{
    bool useGLES;
    
    SDL_GLContext glContext;
    SDL_Window *win = nullptr;
    unsigned int fbo, rbo;
	RenderDetails renderDetails;

public:
    OpenGLRenderer(bool useGLES);
    ~OpenGLRenderer();

    void initialize(const Size size) override;
    std::string getVersion() const override { return std::string(VERSION); }

    SDL_Window *getWindow() const { return win; }
    std::shared_ptr<const Texture> createTexture(const std::string filePath, const Size &textureSize, const void *pixelData) const override;
    void setActiveTexture(std::shared_ptr<const Texture> texture) const override;

    std::shared_ptr<const Model> loadModel(const VertexData &data) const override;

	void beginRender() override;
	void endRender() override;
	void render(const PositionComponent &position, const SpriteComponent &sprite) const override;
    void showMessageBox(const std::string &title, const std::string &message) override;
};


#endif /* OPENGLRENDERER_H */
