#ifndef OPENGLRENDERER_H
#define OPENGLRENDERER_H

#include <SDL.h>
#include <memory>
#include "renderer.h"

#define VERSION "OpenGL Renderer 1.0"

class Entity;
class VertexData;

class OpenGLRenderer : public Renderer
{
    SDL_GLContext glContext;
    SDL_Window *win = nullptr;
    unsigned int fbo, rbo;

    void renderEntities(const std::vector<std::shared_ptr<Entity>> &entities, unsigned int mvpUniform, const glm::mat4 &viewProjection) const;

public:
    OpenGLRenderer();
    ~OpenGLRenderer();

    void initialize(const Size size) override;
    std::string getVersion() const override { return std::string(VERSION); }

    SDL_Window *getWindow() const { return win; }
    std::shared_ptr<const Texture> createTexture(const std::string filePath, const Size &textureSize, const void *pixelData) const override;
    void setActiveTexture(std::shared_ptr<const Texture> texture) const override;

    std::shared_ptr<const Model> loadModel(const VertexData &data) const override;

    void render() const override;
};


#endif /* OPENGLRENDERER_H */
