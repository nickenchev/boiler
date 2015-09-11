#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include "shaderprogram.h"
#include "entity.h"
#include "texture.h"
#include "component.h"

class Renderer : public Component
{
    glm::vec2 globalScale;

protected:
    std::unique_ptr<ShaderProgram> program;

public:
    Renderer(std::string name, const Engine &engine);

    const ShaderProgram *getProgram() const { return program.get(); }

    virtual std::shared_ptr<Texture> createTexture(const Size &textureSize, const void *pixelData) const = 0;
    virtual void setActiveTexture(const Texture &texture) const = 0;
    virtual void render() const = 0;
};


#endif /* RENDERER_H */
