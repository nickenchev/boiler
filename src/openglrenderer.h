#ifndef OPENGLRENDERER_H
#define OPENGLRENDERER_H

#include "renderer.h"

class Entity;

class OpenGLRenderer : public Renderer
{
    SDL_GLContext glContext;
    SDL_Window *win = nullptr;
    unsigned int fbo, rbo;

public:
    OpenGLRenderer(Engine &engine);
    ~OpenGLRenderer();

    SDL_Window *getWindow() const { return win; }
    std::shared_ptr<Texture> createTexture(const Size &textureSize, const void *pixelData) const override;
    void setActiveTexture(const Texture &texture) const override;
    void render() const override;
};


#endif /* OPENGLRENDERER_H */
