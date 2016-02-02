#ifndef OPENGLRENDERER_H
#define OPENGLRENDERER_H

#include <memory>
#include "renderer.h"

class Entity;

class OpenGLRenderer : public Renderer
{
    SDL_GLContext glContext;
    SDL_Window *win = nullptr;
    unsigned int fbo, rbo;

    void renderEntity(const std::shared_ptr<Entity> &entity, unsigned int mvpUniform, const glm::mat4 &viewProjection) const;

public:
    OpenGLRenderer();
    ~OpenGLRenderer();

    SDL_Window *getWindow() const { return win; }
    std::shared_ptr<Texture> createTexture(const std::string filePath, const Size &textureSize, const void *pixelData) const override;
    void setActiveTexture(const Texture &texture) const override;
    void render() const override;
};


#endif /* OPENGLRENDERER_H */
