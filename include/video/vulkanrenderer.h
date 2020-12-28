#ifndef VULKANRENDERER_H
#define VULKANRENDERER_H

#include <string>
#include <optional>
#include <set>
#include <vector>

struct SDL_Window;

#include "core/rect.h"
#include "core/assetmanager.h"
#include "video/vulkan.h"
#include "video/renderer.h"
#include "video/viewprojection.h"

#include "video/vulkan/bufferinfo.h"
#include "video/vulkan/primitivebuffers.h"

#include "video/vulkan/resourceset.h"
#include "video/vulkan/shaderstagemodules.h"
#include "video/vulkan/graphicspipeline.h"

namespace Boiler { namespace Vulkan {

class SPVShaderProgram;

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphics;
	std::optional<uint32_t> presentation;
	std::optional<uint32_t> transfer;
};

struct GBufferPushConstants
{
	vec3 cameraPosition;
};

class VulkanRenderer : public Boiler::Renderer
{
	struct OffscreenBuffer
	{
		VkImage image;
		VkDeviceMemory imageMemory;
		VkImageView imageView;
	};

	struct GBuffer
	{
		OffscreenBuffer positions, albedo, normals;
	};

	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	VkFormat positionFormat, albedoFormat, normalFormat;
	std::vector<GBuffer> gBuffers;

	bool cleanedUp;
	SDL_Window *win;
	bool resizeOccured;
	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkPhysicalDevice physicalDevice;
	VkDevice device;
	VkQueue graphicsQueue, presentationQueue, transferQueue;
	VkSurfaceKHR surface;

	// swapchain related
	VkSwapchainKHR swapChain;
	VkFormat swapChainFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImage> swapChainImages;
	std::vector<VkImageView> swapChainImageViews;

	QueueFamilyIndices queueFamilyIndices;
	std::set<uint32_t> uniqueQueueIndices;
	VkRenderPass renderPass;

	// descriptor related
	struct Descriptor
	{
		VkDescriptorSetLayout layout;
		VkDescriptorPool pool;
		unsigned int count;
		std::vector<VkDescriptorSet> sets;

		void setCount(unsigned int count)
		{
			sets.resize(count);
			this->count = count;
		}
	};
	Descriptor renderDescriptor, attachDescriptor;

	VkPipelineLayout gBuffersPipelineLayout, deferredPipelineLayout, noTexPipelineLayout;
	//VkPipeline gBufferPipeline, deferredPipeline;
	GraphicsPipeline gBufferPipeline, deferredPipeline;

	std::vector<VkFramebuffer> framebuffers;
	VkCommandPool commandPool, transferPool;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkSemaphore> imageSemaphores, renderSemaphores;
	std::vector<VkFence> frameFences;

	// resource management
	AssetManager<PrimitiveBuffers, 512> primitives;
	BufferInfo matrixBuffer, lightsBuffer, materialBuffer;

	void freeBuffer(const BufferInfo &bufferInfo) const;
	// matrices
	void createMatrixBuffer();
	void updateMatrices(const std::vector<mat4> &matrices) const override;
	// lights
	void createLightBuffer(int lightCount);
	void updateLights(const std::vector<LightSource> &lightSources) override;
	// materials
	void createMaterialBuffer();
	void updateMaterials(const std::vector<ShaderMaterial> &materials) const override;

	void test();
	
	
	std::vector<ResourceSet> resourceSets;

	ShaderStageModules gBufferModules, deferredModules, noTexModules;
	short currentFrame;
	uint32_t imageIndex;
	VkResult nextImageResult;
	VkSampler textureSampler;

	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	void createSwapChain();
	void createGBuffers();
	void createRenderBuffers();

	VkShaderModule createShaderModule(const std::vector<char> &contents) const;
	VkRenderPass createRenderPass();
    VkPipelineLayout createGraphicsPipelineLayout(const VkPipelineLayoutCreateInfo &createInfo) const;
	void createGraphicsPipelines();

	void createFramebuffers();
	void createDescriptorSetLayouts();
	template<size_t Size>
	VkDescriptorSetLayout createDescriptorSetLayout(const std::array<VkDescriptorSetLayoutBinding, Size> bindings) const;
	template<size_t Size>
	VkDescriptorPool createDescriptorPool(unsigned int count, const std::array<VkDescriptorPoolSize, Size> &poolSizes) const;
	void allocateDescriptorSets(Descriptor &descriptor);
	void createDescriptorSets();
	VkCommandPool createCommandPools(const QueueFamilyIndices &queueFamilyIndices, const VkQueue &graphicsQueue, const VkQueue &transferQueue);
	void createCommandBuffers();
	void createSynchronization();
	void recreateSwapchain();
	void cleanupSwapchain();
	void createTextureSampler();
	void createDepthResources();

	// memory/buffer operations
	uint32_t findMemoryType(uint32_t filter, VkMemoryPropertyFlags flags) const;
	BufferInfo createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties) const;

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
	BufferInfo createGPUBuffer(void *data, long size, VkBufferUsageFlags usageFlags) const;

public:
    VulkanRenderer(const std::vector<const char *> requiredExtensions);
	~VulkanRenderer();

	// overrides
	std::string getVersion() const override;
	void initialize(const Boiler::Size &size) override;
	void shutdown() override;
	void prepareShutdown() override;
	void resize(const Boiler::Size &size) override;

    Texture loadTexture(const ImageData &imageData) override;
    Primitive loadPrimitive(const VertexData &data) override;
	Material &createMaterial() override;

	void beginRender() override;
	void endRender() override;
	void render(const mat4 modelMatrix, const Primitive &primitive, const Material &material) override;

	// TODO: This needs to be improved
	VkInstance getVulkanInstance() const { return instance; }
	void setSurface(VkSurfaceKHR surface)
	{
		this->surface = surface;
	}
	void setInstance(VkInstance instance)
	{
		this->instance = instance;
	}
};

}
}

#endif /* VULKANRENDERER_H */
