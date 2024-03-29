#ifndef VULKANRENDERER_H
#define VULKANRENDERER_H

#include <string>
#include <optional>
#include <set>
#include <vector>

struct SDL_Window;

#include "core/assetmanager.h"
#include "vulkan.h"
#include "../renderer.h"
#include "bufferinfo.h"
#include "primitivebuffers.h"
#include "textureimage.h"
#include "shaderstagemodules.h"
#include "graphicspipelinebuilder.h"
#include "descriptorset.h"
#include "sampler.h"

namespace Boiler
{
	struct MaterialGroup;
	struct AssetSet;
}

namespace Boiler::Vulkan
{

class SPVShaderProgram;
struct TextureRequest;

struct QueueFamilyIndices
{
	std::optional<uint32_t> graphics;
	std::optional<uint32_t> presentation;
	std::optional<uint32_t> transfer;
};

struct RenderConstants
{
	vec4 offset;
	int matrixId, materialId;
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

	bool enableValidationLayers, cleanedUp, ownedSurface;
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
	DescriptorSet renderDescriptors, materialDescriptors, deferredDescriptors;

	VkPipelineLayout gBuffersPipelineLayout, deferredPipelineLayout, uiPipelineLayout;
	GraphicsPipeline skyboxPipeline, gBufferPipeline, alphaBufferPipeline, gBufferNoTexPipeline, deferredPipeline, uiPipeline, glyphPipeline, debugLinePipeline;

	std::vector<VkFramebuffer> framebuffers;
	VkCommandPool commandPool, transferPool;
	std::vector<VkCommandBuffer> commandBuffers, geometryCommandBuffers, alphaCommandBuffers, deferredCommandBuffers, skyboxCommandBuffers, uiCommandBuffers, debugCommandBuffers;
	std::vector<VkSemaphore> imageSemaphores, renderSemaphores;
	std::vector<VkFence> frameFences;

	// resource management
	AssetManager<BufferInfo, 2048> buffers;
	AssetManager<PrimitiveBuffers, 2048> primitives;
	AssetManager<TextureImage, 512> textures;
	AssetId matrixBufferId, lightsBufferId, materialBufferId;
	void updateLights(const std::vector<LightSource> &lightSources) const override;
	void updateMaterials(const std::vector<ShaderMaterial> &materials) const override;

	ShaderStageModules gBufferModules, deferredModules, skyboxModules, uiModules, glyphModules, debugModules;
	uint32_t imageIndex;
	VkResult nextImageResult;
	Sampler textureSampler, cubemapSampler, glyphAtlasSampler;

	std::vector<TextureImage> depthImages;

	void createSwapChain();
	void createGBuffers();

	VkShaderModule createShaderModule(const std::vector<char> &contents) const;
	VkRenderPass createRenderPass();
    VkPipelineLayout createGraphicsPipelineLayout(const VkPipelineLayoutCreateInfo &createInfo) const;
	void createGraphicsPipelines();
	void createFramebuffers();

	// descriptor set related
	void createDescriptorSets();

	VkCommandPool createCommandPools(const QueueFamilyIndices &queueFamilyIndices, const VkQueue &graphicsQueue, const VkQueue &transferQueue);
	void createCommandBuffers();
	void createSecondaryCommandBuffers(std::vector<VkCommandBuffer> &secondaryBuffers);
	void createSynchronization();
	void recreateSwapchain();
	void cleanupSwapchain();
	void createTextureSamplers();
	void createDepthResources();

	// memory/buffer operations
	VkDeviceSize offsetUniform(VkDeviceSize offset);
	uint32_t findMemoryType(uint32_t filter, VkMemoryPropertyFlags flags) const;

	AssetId createBuffer(size_t size, BufferUsage usage, MemoryType memType) override;
	void freeBuffer(const BufferInfo &bufferInfo) const;
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize dataSize) const;

	VkCommandBuffer beginSingleTimeCommands(const VkCommandPool &pool) const;
	void endSingleTimeCommands(const VkQueue &queue, const VkCommandPool &pool, const VkCommandBuffer &buffer) const;

	// image operations
	std::pair<VkImage, VkDeviceMemory> createImage(const TextureRequest &request) const;
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags,
								VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D) const;
	void transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout,
							   unsigned int arrayLayer = 0, unsigned int layerCount = 1) const;
	void copyBufferToImage(VkBuffer buffer, size_t bufferOffset, VkImage image, const Size &imageSize,
						   unsigned int arrayLayer = 0, unsigned int layerCount = 1) const;
	VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling,
								 VkFormatFeatureFlags features) const;
	VkFormat findDepthFormat() const;
	bool hasStencilComponent(VkFormat format) const;
	AssetId createGPUBuffer(void *data, size_t size, BufferUsage usage);

public:
    VulkanRenderer(const std::vector<const char *> requiredExtensions, bool enableValidationLayers = false);
	~VulkanRenderer();

	// overrides
	std::string getVersion() const override;
	void initialize(const Boiler::Size &size) override;
	void shutdown() override;
	void prepareShutdown() override;
	void resize(const Boiler::Size &size) override;
	void setOwnedSurface(bool owned) { ownedSurface = owned; }

	AssetId loadTexture(const ImageData &imageData, TextureType type) override;
	AssetId loadCubemap(const std::array<ImageData, 6> &images) override;
	AssetId loadPrimitive(const VertexData &data, AssetId existingId = Asset::noAsset()) override;

	bool prepareFrame(const FrameInfo &frameInfo) override;
	void render(AssetSet &assetSet, const FrameInfo &frameInfo,
				const std::vector<MaterialGroup> &materialGroups, RenderStage stage) override;
	void finalizeFrame(const FrameInfo &frameInfo, AssetSet &assetSet) override;

	// TODO: This needs to be improved
	VkInstance getVulkanInstance() const { return instance; }
	VkDevice getDevice() const { return device; }
	VkPhysicalDevice getPhysicalDevice() const { return physicalDevice; }
	VkQueue getGraphicsQueue() const { return graphicsQueue; }

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

#endif /* VULKANRENDERER_H */
