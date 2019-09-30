#ifndef VULKANRENDERER_H
#define VULKANRENDERER_H

#include <string>
#include <optional>
#include <set>

#define VK_USE_PLATFORM_XCB_KHR
#include <vulkan/vulkan.h>

#include "video/renderer.h"

class SDL_Window;

namespace Boiler
{

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphics;
	std::optional<uint32_t> presentation;
};

class VulkanRenderer : public Boiler::Renderer
{
	SDL_Window *win;
	bool resizeOccured;
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkQueue graphicsQueue, presentationQueue;
	VkSurfaceKHR surface;
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
	VkFormat swapChainFormat;
	VkExtent2D swapChainExtent;
	QueueFamilyIndices queueFamilyIndices;
	std::set<uint32_t> uniqueQueueIndices;
	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	std::vector<VkFramebuffer> framebuffers;
	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkSemaphore> imageSemaphores, renderSemaphores;
	std::vector<VkFence> frameFences;
	short currentFrame;

	void createSwapChain();
	void createRenderPass();
	void createGraphicsPipeline();
	void createFramebuffers();

	void createCommandPool();
	void createCommandBuffers();
	void createSynchronization();

	void cleanupSwapchain();
	void recreateSwapchain();

public:
    VulkanRenderer();
	~VulkanRenderer();

	void initialize(const Boiler::Size &size) override;
	void resize(const Boiler::Size &size) override;
	void shutdown() override;
	std::string getVersion() const override;

    std::shared_ptr<const Texture> createTexture(const std::string filePath, const Size &textureSize, const void *pixelData) const override;
    void setActiveTexture(std::shared_ptr<const Texture> texture) const override;

    std::shared_ptr<const Model> loadModel(const VertexData &data) const override;

	void beginRender() override;
	void endRender() override;

	void render(const glm::mat4 modelMatrix, const std::shared_ptr<const Model> model,
				const std::shared_ptr<const Texture> sourceTexture, const TextureInfo *textureInfo,
				const glm::vec4 &colour) override;

    void showMessageBox(const std::string &title, const std::string &message) override;
};

}

#endif /* VULKANRENDERER_H */
