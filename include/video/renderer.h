#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include <glm/glm.hpp>
#include "shaderprogram.h"
#include "core/logger.h"
#include "core/entity.h"
#include "core/component.h"
#include "core/rect.h"
#include "texture.h"

namespace Boiler
{

typedef glm::vec3 Color3;
typedef glm::vec4 Color4;

class Model;
class VertexData;
struct PositionComponent;
struct SpriteComponent;
struct TextComponent;
class TextureInfo;

class Renderer
{
    glm::vec2 globalScale;
    Color3 clearColor;

protected:
	Logger logger;
    Size screenSize;
    std::unique_ptr<ShaderProgram> program;

public:
    Renderer(std::string name);
	virtual ~Renderer() { }

    virtual void initialize(const Size size) = 0;
    virtual void shutdown() = 0;
    virtual std::string getVersion() const = 0;

    void setScreenSize(const Size &screenSize) { this->screenSize = screenSize; }
    const Size &getScreenSize() const { return screenSize; }
    const ShaderProgram *getProgram() const { return program.get(); }

    const glm::vec2 &getGlobalScale() const { return globalScale; }
    void setGlobalScale(const glm::vec2 &scale) { this->globalScale = scale; }
    const Color3 &getClearColor() const { return clearColor; }
    void setClearColor(const Color3 &color) { clearColor = color; }

    virtual std::shared_ptr<const Texture> createTexture(const std::string filePath, const Size &textureSize, const void *pixelData) const = 0;
    virtual void setActiveTexture(std::shared_ptr<const Texture> texture) const = 0;
    virtual std::shared_ptr<const Model> loadModel(const VertexData &data) const = 0;

	virtual void beginRender() = 0;
	virtual void endRender() = 0;

	virtual void render(const glm::mat4 modelMatrix, const std::shared_ptr<const Model> model,
						const std::shared_ptr<const Texture> sourceTexture, const TextureInfo *textureInfo,
						const glm::vec4 &colour) const = 0;

    virtual void showMessageBox(const std::string &title, const std::string &message) = 0;
};

}

#endif /* RENDERER_H */
