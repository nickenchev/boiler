#ifndef OPENGLRENDERER_H
#define OPENGLRENDERER_H

#include "renderer.h"

class Entity;

class OpenGLRenderer : public Renderer
{
public:
    OpenGLRenderer(Engine &engine);

    std::shared_ptr<Texture> createTexture(const Size &textureSize, const void *pixelData) const override;
    void setActiveTexture(const Texture &texture) const override;
    void render() const override;
};


#endif /* OPENGLRENDERER_H */
