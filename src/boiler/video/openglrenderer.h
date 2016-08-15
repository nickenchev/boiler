#ifndef OPENGLRENDERER_H
#define OPENGLRENDERER_H

#include <SDL2/SDL.h>
#include <memory>
#include "renderer.h"

#define COMPONENT_NAME "OpenGL Renderer"
#define VERSION COMPONENT_NAME

class Entity;
class VertexData;

struct RenderDetails
{
    GLuint mvpUniform;
    GLuint colorUniform;
    GLboolean usingTexUniform;
    glm::mat4 viewProjection;

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

    void renderEntities(const std::vector<std::shared_ptr<Entity>> &entities, const RenderDetails &renderDetails) const;

public:
    OpenGLRenderer(bool useGLES);
    ~OpenGLRenderer();

    void initialize(const Size size) override;
    std::string getVersion() const override { return std::string(VERSION); }

    SDL_Window *getWindow() const { return win; }
    std::shared_ptr<const Texture> createTexture(const std::string filePath, const Size &textureSize, const void *pixelData) const override;
    void setActiveTexture(std::shared_ptr<const Texture> texture) const override;

    std::shared_ptr<const Model> loadModel(const VertexData &data) const override;

    void render() const override;
    void showMessageBox(const std::string &title, const std::string &message) override;
};


#endif /* OPENGLRENDERER_H */
