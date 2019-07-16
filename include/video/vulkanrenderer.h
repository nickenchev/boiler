#ifndef VULKANRENDERER_H
#define VULKANRENDERER_H

#include <string>
#include <vulkan/vulkan.h>
#include "video/renderer.h"

class SDL_Window;

namespace Boiler
{

class VulkanRenderer : public Boiler::Renderer
{
	SDL_Window *win;
	VkInstance instance;
public:
    VulkanRenderer();

	void initialize(const Boiler::Size &size) override;
	void shutdown() override;
	std::string getVersion() const override;

    std::shared_ptr<const Texture> createTexture(const std::string filePath, const Size &textureSize, const void *pixelData) const override;
    void setActiveTexture(std::shared_ptr<const Texture> texture) const override;

    std::shared_ptr<const Model> loadModel(const VertexData &data) const override;

	void beginRender() override;
	void endRender() override;

	void render(const glm::mat4 modelMatrix, const std::shared_ptr<const Model> model,
				const std::shared_ptr<const Texture> sourceTexture, const TextureInfo *textureInfo,
				const glm::vec4 &colour) const override;

    void showMessageBox(const std::string &title, const std::string &message) override;
};

}

#endif /* VULKANRENDERER_H */
