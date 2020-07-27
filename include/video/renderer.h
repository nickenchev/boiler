#ifndef RENDERER_H
#define RENDERER_H

#include "core/logger.h"
#include "core/entity.h"
#include "core/asset.h"
#include "core/component.h"
#include "core/rect.h"
#include "core/math.h"
#include "texture.h"

namespace Boiler
{

class Primitive;
class VertexData;
class TextureInfo;
struct PositionComponent;
struct SpriteComponent;
struct TextComponent;
struct ImageData;

class Renderer
{
    vec2 globalScale;
    vec3 clearColor;
	AssetId assetId;

protected:
	Logger logger;
    Size screenSize;
	glm::mat4 viewMatrix;

	AssetId nextAssetId();

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

    virtual Texture loadTexture(const std::string &filePath, const ImageData &imageData) = 0;
    virtual Primitive loadPrimitive(const VertexData &data) = 0;

	virtual void beginRender() = 0;
	virtual void endRender() = 0;
	virtual void render(const mat4 modelMatrix, const Primitive &primitive, const Texture &sourceTexture, const vec4 &colour) = 0;
};

}

#endif /* RENDERER_H */
