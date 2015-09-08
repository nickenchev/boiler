#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include "entity.h"
#include "texture.h"
#include "component.h"

class Renderer : public Component
{
public:
    Renderer(std::string name, const Engine &engine);

    virtual std::shared_ptr<Texture> createTexture(const Size &textureSize, const void *pixelData) const = 0;
    virtual void setActiveTexture(const Texture &texture) const = 0;
    virtual void render() const = 0;
};


#endif /* RENDERER_H */
