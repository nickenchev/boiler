#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include "shaderprogram.h"
#include "entity.h"
#include "texture.h"
#include "component.h"

typedef glm::vec3 Color3;

class Camera;

class Renderer : public Component
{
    glm::vec2 globalScale;
    Color3 clearColor;

protected:
    std::unique_ptr<ShaderProgram> program;
    std::shared_ptr<Camera> camera;

public:
    Renderer(std::string name, Engine &engine);

    const ShaderProgram *getProgram() const { return program.get(); }
    void setCamera(std::shared_ptr<Camera> camera) { this->camera = camera; }

    const glm::vec2 &getGlobalScale() const { return globalScale; }
    void setGlobalScale(const glm::vec2 &scale) { this->globalScale = scale; }
    const Color3 &getClearColor() const { return clearColor; }
    void setClearColor(const Color3 &color) { clearColor = color; }

    virtual std::shared_ptr<Texture> createTexture(const Size &textureSize, const void *pixelData) const = 0;
    virtual void setActiveTexture(const Texture &texture) const = 0;
    virtual void render() const = 0;
};


#endif /* RENDERER_H */