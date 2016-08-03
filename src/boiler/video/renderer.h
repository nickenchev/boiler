#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include "shaderprogram.h"
#include "../core/entity.h"
#include "../core/component.h"
#include "texture.h"

typedef glm::vec3 Color3;
typedef glm::vec4 Color4;

class Camera;
class Model;
class VertexData;

class Renderer : public Component
{
    glm::vec2 globalScale;
    Color3 clearColor;

protected:
    std::unique_ptr<ShaderProgram> program;
    std::shared_ptr<Camera> camera;

public:
    Renderer(std::string name);

    virtual void initialize(const Size size) = 0;
    virtual std::string getVersion() const = 0;

    const ShaderProgram *getProgram() const { return program.get(); }
    void setCamera(std::shared_ptr<Camera> camera) { this->camera = camera; }

    const glm::vec2 &getGlobalScale() const { return globalScale; }
    void setGlobalScale(const glm::vec2 &scale) { this->globalScale = scale; }
    const Color3 &getClearColor() const { return clearColor; }
    void setClearColor(const Color3 &color) { clearColor = color; }

    // textures
    virtual std::shared_ptr<const Texture> createTexture(const std::string filePath, const Size &textureSize, const void *pixelData) const = 0;
    virtual void setActiveTexture(std::shared_ptr<const Texture> texture) const = 0;

    // models
    virtual std::shared_ptr<const Model> loadModel(const VertexData &data) const = 0;
    
    virtual void render() const = 0;
};


#endif /* RENDERER_H */
