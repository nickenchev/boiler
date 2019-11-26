#ifndef VULKANRENDERER_H
#define VULKANRENDERER_H

#include <string>
#include <optional>
#include <set>

#include "core/rect.h"
#include "video/vulkan.h"
#include "video/renderer.h"
#include "vulkan/vulkan_core.h"

class SDL_Window;

namespace Boiler
{

class SPVShaderProgram;

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphics;
	std::optional<uint32_t> presentation;
	std::optional<uint32_t> transfer;
};

class VulkanRenderer : public Boiler::Renderer
{
	SDL_Window *win;
	bool resizeOccured;
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkQueue graphicsQueue, presentationQueue, transferQueue;
	VkSurfaceKHR surface;
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;
	VkFormat swapChainFormat;
	VkExtent2D swapChainExtent;
	QueueFamilyIndices queueFamilyIndices;
	std::set<uint32_t> uniqueQueueIndices;
	VkRenderPass renderPass;
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	std::vector<VkFramebuffer> framebuffers;
	VkCommandPool commandPool, transferPool;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkSemaphore> imageSemaphores, renderSemaphores;
	std::vector<VkFence> frameFences;
	std::vector<VkBuffer> mvpBuffers;
	std::vector<VkDeviceMemory> mvpBuffersMemory;
	std::unique_ptr<SPVShaderProgram> program;
	short currentFrame;
	uint32_t imageIndex;
	VkResult nextImageResult;

	void createSwapChain();
	void createRenderPass();
	void createGraphicsPipeline();
	void createFramebuffers();
	void createDescriptorSetLayout();
	void createDescriptorPool();
	void createDescriptorSets();

	void createCommandPools();
	void createCommandBuffers();
	void createSynchronization();
	void createMvpBuffers();

	void recreateSwapchain();
	void cleanupSwapchain();

	// memory/buffer operations
	uint32_t findMemoryType(uint32_t filter, VkMemoryPropertyFlags flags) const;
	std::pair<VkBuffer, VkDeviceMemory> createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
													 VkMemoryPropertyFlags memoryProperties) const;

	VkCommandBuffer beginSingleTimeCommands() const;
	void endSingleTimeCommands(const VkCommandBuffer &commandBuffer) const;
	void copyBuffer(VkBuffer &srcBuffer, VkBuffer dstBuffer, VkDeviceSize dataSize) const;

	// image operations
	std::pair<VkImage, VkDeviceMemory> createImage(const Size &imageSize, VkFormat format, VkImageTiling tiling,
												   VkImageUsageFlags usage, VkMemoryPropertyFlags memProperties) const;
	void transitionImageLayout(VkImage image, VkFormat format,
							   VkImageLayout oldLayout, VkImageLayout newLayout) const;
	void copyBufferToImage(VkBuffer buffer, VkImage image, const Size &imageSize) const;

public:
    VulkanRenderer();
	~VulkanRenderer();

	void initialize(const Boiler::Size &size) override;
	void resize(const Boiler::Size &size) override;
	std::string getVersion() const override;

    std::shared_ptr<const Texture> createTexture(const std::string &filePath, const Size &textureSize,
												 const void *pixelData, u_int8_t bytesPerPixel) const override;
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
