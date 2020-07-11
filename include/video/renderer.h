#ifndef RENDERER_H
#define RENDERER_H

#include <memory>
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
	glm::mat4 viewMatrix;

public:
    Renderer(std::string name);
	virtual ~Renderer() { }

    virtual void initialize(const Size &size);
	virtual void prepareShutdown() = 0;
	virtual void resize(const Size &size);
    virtual std::string getVersion() const = 0;

    void setScreenSize(const Size &screenSize) { this->screenSize = screenSize; }
    const Size &getScreenSize() const { return screenSize; }

    const vec2 &getGlobalScale() const { return globalScale; }
    void setGlobalScale(const vec2 &scale) { this->globalScale = scale; }
    const vec3 &getClearColor() const { return clearColor; }
    void setClearColor(const vec3 &color) { clearColor = color; }
	void setViewMatrix(const glm::mat4 &viewMatrix) { this->viewMatrix = viewMatrix; }

    virtual std::shared_ptr<const Texture> createTexture(const std::string &filePath, const Size &textureSize,
														 const void *pixelData, uint8_t bytesPerPixel) const = 0;
    virtual std::shared_ptr<const Model> loadModel(const VertexData &data) const = 0;
	virtual unsigned int createLight() = 0;

	virtual void beginRender() = 0;
	virtual void endRender() = 0;

	virtual void render(const mat4 modelMatrix, const std::shared_ptr<const Model> model,
						const std::shared_ptr<const Texture> sourceTexture, const TextureInfo *textureInfo,
						const vec4 &colour) = 0;

    virtual void showMessageBox(const std::string &title, const std::string &message) = 0;
};

}

#endif /* RENDERER_H */
