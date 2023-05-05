#pragma once

#include <display/renderer.h>

namespace Boiler
{

class OpenGLRenderer : public Renderer
{
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
