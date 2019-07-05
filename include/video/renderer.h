#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
#include "shaderprogram.h"
#include "core/logger.h"
#include "core/entity.h"
#include "core/component.h"
#include "core/rect.h"
#include "core/math.h"
#include "texture.h"

namespace Boiler
{

class Model;
class VertexData;
struct PositionComponent;
struct SpriteComponent;
struct TextComponent;
class TextureInfo;

class Renderer
{
    vec2 globalScale;
    vec3 clearColor;

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

    const vec2 &getGlobalScale() const { return globalScale; }
    void setGlobalScale(const vec2 &scale) { this->globalScale = scale; }
    const vec3 &getClearColor() const { return clearColor; }
    void setClearColor(const vec3 &color) { clearColor = color; }

    virtual std::shared_ptr<const Texture> createTexture(const std::string filePath, const Size &textureSize, const void *pixelData) const = 0;
    virtual void setActiveTexture(std::shared_ptr<const Texture> texture) const = 0;
    virtual std::shared_ptr<const Model> loadModel(const VertexData &data) const = 0;

	virtual void beginRender() = 0;
	virtual void endRender() = 0;

	virtual void render(const mat4 modelMatrix, const std::shared_ptr<const Model> model,
						const std::shared_ptr<const Texture> sourceTexture, const TextureInfo *textureInfo,
						const vec4 &colour) const = 0;

    virtual void showMessageBox(const std::string &title, const std::string &message) = 0;
};

}

#endif /* RENDERER_H */
