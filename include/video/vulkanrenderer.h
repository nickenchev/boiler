#ifndef VULKANRENDERER_H
#define VULKANRENDERER_H

#include <string>
#include <optional>
#include <set>
#include <vector>

#include "core/rect.h"
#include "video/vulkan.h"
#include "video/renderer.h"
#include "video/vulkan/resourceset.h"
#include "vulkan/vulkan_core.h"

class SDL_Window;

namespace Boiler { namespace Vulkan {

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
	VkRenderPass renderPass, renderPass2;
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

	// resource management
	std::vector<ResourceSet> resourceSets;

	std::unique_ptr<SPVShaderProgram> program;
	short currentFrame;
	uint32_t descriptorCount;
	uint32_t imageIndex;
	VkResult nextImageResult;
	VkSampler textureSampler;

	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	void createComponents();

	void createSwapChain();
	VkRenderPass createRenderPass();
	VkPipelineLayout createGraphicsPipelineLayout(VkDescriptorSetLayout descriptorSetLayout) const;
	VkPipeline createGraphicsPipeline(VkRenderPass renderPass, VkPipelineLayout pipelineLayout, VkExtent2D swapChainExtent, const SPVShaderProgram &program) const;

	void createFramebuffers();
	VkDescriptorSetLayout createDescriptorSetLayout() const;
	void createDescriptorPool();
	void createDescriptorSets();
	VkCommandPool createCommandPools(const QueueFamilyIndices &queueFamilyIndices, const VkQueue &graphicsQueue, const VkQueue &transferQueue);
	void createCommandBuffers();
	void createSynchronization();
	void createMvpBuffers();
	void recreateSwapchain();
	void cleanupSwapchain();
	void createTextureSampler();
	void createDepthResources();

	// memory/buffer operations
	uint32_t findMemoryType(uint32_t filter, VkMemoryPropertyFlags flags) const;
	std::pair<VkBuffer, VkDeviceMemory> createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
													 VkMemoryPropertyFlags memoryProperties) const;

	VkCommandBuffer beginSingleTimeCommands(const VkCommandPool &pool) const;
	void endSingleTimeCommands(const VkQueue &queue, const VkCommandPool &pool, const VkCommandBuffer &buffer) const;
	void copyBuffer(VkBuffer &srcBuffer, VkBuffer dstBuffer, VkDeviceSize dataSize) const;

	// image operations
	std::pair<VkImage, VkDeviceMemory> createImage(const Size &imageSize, VkFormat format, VkImageTiling tiling,
												   VkImageUsageFlags usage, VkMemoryPropertyFlags memProperties) const;
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) const;
	void transitionImageLayout(VkImage image, VkFormat format,
							   VkImageLayout oldLayout, VkImageLayout newLayout) const;
	void copyBufferToImage(VkBuffer buffer, VkImage image, const Size &imageSize) const;
	VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling,
								 VkFormatFeatureFlags features) const;
	VkFormat findDepthFormat() const;
	bool hasStencilComponent(VkFormat format) const;

public:
    VulkanRenderer(const std::vector<const char *> requiredExtensions);
	~VulkanRenderer();

	// TODO: This needs to be improved
	VkInstance getVulkanInstance() const { return instance; }
	void setSurface(VkSurfaceKHR surface)
	{
		this->surface = surface;
	}

	void initialize(const Boiler::Size &size) override;
	void prepareShutdown() override;
	void resize(const Boiler::Size &size) override;
	std::string getVersion() const override;

    std::shared_ptr<const Texture> createTexture(const std::string &filePath, const Size &textureSize,
												 const void *pixelData, u_int8_t bytesPerPixel) const override;

	std::pair<VkBuffer, VkDeviceMemory> createGPUBuffer(void *data, long size, VkBufferUsageFlags usageFlags) const;
    std::shared_ptr<const Model> loadModel(const VertexData &data) override;

	void beginRender() override;
	void endRender() override;

	void render(const glm::mat4 modelMatrix, const std::shared_ptr<const Model> model,
				const std::shared_ptr<const Texture> sourceTexture, const TextureInfo *textureInfo,
				const glm::vec4 &colour) override;
};

}
}

#endif /* VULKANRENDERER_H */
