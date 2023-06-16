#pragma once

#include <display/renderer.h>
#include <display/primitive.h>
#include <display/opengl/opengl.h>
#include "opengltexture.h"

namespace Boiler
{

class PrimitiveBuffers : public Asset
{
	GLuint vertexArrayObject, vertexBuffer, indexBuffer;
	unsigned int indices;

public:
	PrimitiveBuffers()
	{
		vertexArrayObject = 0;
		vertexBuffer = 0;
		indexBuffer = 0;
		indices = 0;
	}

	PrimitiveBuffers(GLuint vertexArrayObject, GLuint vertexBuffer, GLuint indexBuffer)
	{
		this->vertexArrayObject = vertexArrayObject;
		this->vertexBuffer = vertexBuffer;
		this->indexBuffer = indexBuffer;
	}

	GLuint getVertexArrayObject() const { return vertexArrayObject; }
	GLuint getVertexBuffer() const { return vertexBuffer; }
	GLuint getIndexBuffer() const { return indexBuffer; }
};

class OpenGLRenderer : public Renderer
{
	Boiler::AssetManager<PrimitiveBuffers, 2048> primitiveBuffers;
	Boiler::AssetManager<Boiler::Primitive, 2048> primitives;
	AssetManager<OpenGLTexture, 512> textures;

public:
    OpenGLRenderer();
    ~OpenGLRenderer() override;

    void initialize(const Size &size) override;
    void shutdown() override;
    void prepareShutdown() override;
    void resize(const Size &size) override;
    std::string getVersion() const override;
    AssetId loadTexture(const ImageData &imageData, TextureType type) override;
    AssetId loadCubemap(const std::array<ImageData, 6> &images) override;
    AssetId loadPrimitive(const VertexData &data, AssetId existingId) override;
    AssetId createBuffer(size_t size, BufferUsage usage, MemoryType memType) override;
    void updateLights(const std::vector<LightSource> &lightSources) const override;
    void updateMaterials(const std::vector<ShaderMaterial> &materials) const override;
    bool prepareFrame(const FrameInfo &frameInfo) override;
    void render(AssetSet &assetSet, const FrameInfo &frameInfo, const std::vector<MaterialGroup> &materialGroups,
                RenderStage stage) override;
    void displayFrame(const FrameInfo &frameInfo, AssetSet &assetSet) override;
};

}
