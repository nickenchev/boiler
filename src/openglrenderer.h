#ifndef OPENGLRENDERER_H
#define OPENGLRENDERER_H

#include "renderer.h"

class OpenGLRenderer : public Renderer
{
public:
    OpenGLRenderer(Engine &engine);

    std::shared_ptr<Texture> createTexture(const Size &textureSize, const void *pixelData) const override;
    void setActiveTexture(const Texture &texture) const override;
};


#endif /* OPENGLRENDERER_H */
