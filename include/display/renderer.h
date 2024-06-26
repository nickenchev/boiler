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
#include "core/assetmanager.h"

#define MAX_OBJECTS 4000

namespace Boiler
{

enum class TextureType
{
	FREETYPE_ATLAS,
	RGB_SRGB,
	RGBA_SRGB,
	RGBA_UNORM
};

enum class RenderStage
{
	PRE_DEFERRED_LIGHTING,
	ALPHA_BLENDING,
	POST_DEFERRED_LIGHTING,
	POST_DEPTH_WRITE,
	UI,
	TEXT,
	DEBUG
};

enum class BufferUsage
{
	STAGING,
	UNIFORM,
	VERTICES,
	INDICES
};

enum class MemoryType
{
	DEVICE_LOCAL,
	HOST_CACHED,
	DEVICE_LOCAL_HOST_CACHED
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
	unsigned short maxFramesInFlight;

protected:
	Logger logger;

    Size screenSize;
	vec3 cameraPosition;
	glm::mat4 viewMatrix;
	unsigned int frameLightIdx;
	AssetManager<mat4, MAX_OBJECTS> matrices;

public:
    Renderer(std::string name, unsigned short maxFramesInflight);
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
	AssetId addMatrix(mat4 matrix);

	virtual AssetId loadTexture(const ImageData &imageData, TextureType type) = 0;
	virtual void deleteTexture(const AssetId textureId) = 0;
	virtual AssetId loadCubemap(const std::array<ImageData, 6> &images) = 0;
	virtual AssetId loadPrimitive(const VertexData &data, AssetId existingId = Asset::noAsset()) = 0;
	virtual AssetId createBuffer(size_t size, BufferUsage usage, MemoryType memType) = 0;

	virtual void updateLights(const std::vector<LightSource> &lightSources) const = 0;
	virtual void updateMaterials(const std::vector<ShaderMaterial> &materials) const = 0;

	virtual bool prepareFrame(const FrameInfo &frameInfo);
	virtual void render(AssetSet &assetSet, const FrameInfo &frameInfo,
						const std::vector<MaterialGroup> &materialGroups, RenderStage stage) = 0;
	virtual void finalizeFrame(const FrameInfo &frameInfo, AssetSet &assetSet) = 0;
	unsigned short getMaxFramesInFlight() const { return maxFramesInFlight; }
	virtual void releaseAssetSet(AssetSet &assetSet) = 0;
};

}

#endif /* RENDERER_H */
