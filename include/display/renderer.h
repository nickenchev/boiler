#ifndef RENDERER_H
#define RENDERER_H

#include <array>

#include "core/logger.h"
#include "core/frameinfo.h"
#include "core/entity.h"
#include "core/asset.h"
#include "core/component.h"
#include "core/rect.h"
#include "core/math.h"
#include "display/material.h"
#include "lightsource.h"
#include "texture.h"
#include "shadermaterial.h"
#include "display/materialgroup.h"

namespace Boiler
{

enum class TextureType
{
	GLYPH_ATLAS,
	RGBA_SRGB
};

class Primitive;
struct Material;
class VertexData;
class TextureInfo;
struct PositionComponent;
struct SpriteComponent;
struct TextComponent;
struct ImageData;
struct AssetSet;
	
class Renderer
{
    vec3 clearColor;
    vec2 globalScale;
	AssetId assetId;

protected:
	Logger logger;

    Size screenSize;
	vec3 cameraPosition;
	glm::mat4 viewMatrix;
	unsigned int frameLightIdx;

public:
    Renderer(std::string name);
	virtual ~Renderer() { }

    virtual void initialize(const Size &size);
	virtual void shutdown() = 0;
	virtual void prepareShutdown() = 0;
	virtual void resize(const Size &size);
    virtual std::string getVersion() const = 0;

    void setScreenSize(const Size &screenSize) { this->screenSize = screenSize; }
    const Size &getScreenSize() const { return screenSize; }

    const vec2 &getGlobalScale() const { return globalScale; }
    void setGlobalScale(const vec2 &scale) { this->globalScale = scale; }
    const vec3 &getClearColor() const { return clearColor; }
    void setClearColor(const vec3 &color) { clearColor = color; }
	void setCameraPosition(const vec3 &cameraPosition) { this->cameraPosition = cameraPosition; }
	void setViewMatrix(const glm::mat4 &viewMatrix) { this->viewMatrix = viewMatrix; }

    virtual AssetId loadTexture(const ImageData &imageData, TextureType type) = 0;
	virtual AssetId loadCubemap(const std::array<ImageData, 6> &images) = 0;
	virtual AssetId loadPrimitive(const VertexData &data) = 0;

	virtual void updateMatrices(const std::vector<mat4> &matrices) const = 0;
	virtual void updateLights(const std::vector<LightSource> &lightSources) = 0;
	virtual void updateMaterials(const std::vector<ShaderMaterial> &materials) const = 0;

	virtual bool prepareFrame(const FrameInfo &frameInfo);
	virtual void displayFrame(const FrameInfo &frameInfo) = 0;
	virtual void render(AssetSet &assetSet, const FrameInfo &frameInfo,
						const std::vector<mat4> &matrices,
						const std::vector<MaterialGroup> &materialGroups,
						const std::vector<MaterialGroup> &postLightGroups) = 0;
};

}

#endif /* RENDERER_H */
