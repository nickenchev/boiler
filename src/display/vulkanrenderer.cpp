#include "display/vulkanrenderer.h"
#include "display/renderer.h"

#include <filesystem>
#include <array>
#include <chrono>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <sys/types.h>
#include <utility>
#include <vector>
#include <fmt/format.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "vulkan/vulkan_core.h"

#include "core/logger.h"
#include "core/math.h"
#include "assets/assetset.h"
#include "display/lightsource.h"
#include "display/imaging/imagedata.h"
#include "display/vertexdata.h"
#include "display/primitive.h"
#include "util/filemanager.h"
#include "display/material.h"
#include "display/materialgroup.h"
#include "display/vulkan/texturerequest.h"
#include "display/glyphmap.h"
#include "display/rendermatrices.h"

using namespace Boiler;
using namespace Boiler::Vulkan;

constexpr bool enableDebugMessages = true;
constexpr VkPresentModeKHR preferredPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
//constexpr VkPresentModeKHR preferredPresentMode = VK_PRESENT_MODE_FIFO_KHR;
constexpr unsigned int maxLights = 64;
constexpr unsigned int maxMaterials = 512;
constexpr unsigned int maxSamplers = 1;
//constexpr VkFormat textureFormat = VK_FORMAT_R8G8B8A8_UNORM;
constexpr VkFormat textureFormat = VK_FORMAT_R8G8B8A8_SRGB;

constexpr size_t DSET_IDX_FRAME = 0;
constexpr size_t DSET_IDX_MATERIAL = 1;

const std::vector<const char *> validationLayers = { "VK_LAYER_KHRONOS_validation" };

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
													VkDebugUtilsMessageTypeFlagsEXT messageType,
													const VkDebugUtilsMessengerCallbackDataEXT *callbackData,
													void *userData);

template<typename Function>
Function getFunctionPointer(VkInstance instance, std::string funcName)
{
	Function func = reinterpret_cast<Function>(vkGetInstanceProcAddr(instance, funcName.c_str()));
	if (!func)
	{
		throw std::runtime_error("Couldn't find Vulkan function: " + funcName);
	}
	return func;
}

VulkanRenderer::VulkanRenderer(const std::vector<const char *> requiredExtensions, bool enableValidationLayers) : Renderer("Vulkan Renderer", 3)
{
	this->enableValidationLayers = enableValidationLayers;
	cleanedUp = false;
	resizeOccured = false;
	commandPool = VK_NULL_HANDLE;

	// query supported extensions
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> instProps(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, instProps.data());

	// copy required extensions into our final requested list
	std::vector<const char *> requestedExtensions;
	for (const char *ext : requiredExtensions)
	{
		requestedExtensions.push_back(ext);
	}

	// user-requested Vulkan extensions
	requestedExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

	// check if extensions are supported
	bool extensionsOk = true;
	for (auto extension : requestedExtensions)
	{
		bool supported = false;
		for (int i = 0; i < static_cast<int>(extensionCount); ++i)
		{
			if (std::strcmp(extension, instProps[i].extensionName) == 0)
			{
				supported = true;
                logger.log("{} will be enabled", instProps[i].extensionName);
			}
		}
		if (!supported)
		{
			extensionsOk = false;
			throw std::runtime_error("Unsupported extension: " + std::string(extension));
			break;
		}
	}
	assert(extensionsOk);
    logger.log("{} extensions supported total", extensionCount);

	// create instance
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Boiler Vulkan Renderer";
	appInfo.pEngineName = "Boiler";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_2;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = static_cast<unsigned int>(requestedExtensions.size());
	createInfo.ppEnabledExtensionNames = requestedExtensions.data();
	createInfo.enabledLayerCount = 0;

	// validation layers
	uint32_t layerCount = 0;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	std::vector<VkLayerProperties> layers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

	if (enableValidationLayers)
	{
		// check that requested layers are supported
		bool layersOk = true;
		for (auto layerName : validationLayers)
		{
			bool layerFound = false;
			for (auto availLayer : layers)
			{
				if (std::strcmp(layerName, availLayer.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
			{
				layersOk = false;
				logger.error("Layer: {} not found", layerName);
			}
		}
		assert(layersOk);

		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		logger.log(std::to_string(validationLayers.size()) + " layer(s) will be enabled");
		logger.log("Validation layers are enabled");
	}

	// create the vulkan instance
	if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
	{
		throw std::runtime_error("Error creating Vulkan instance");
	}
	else
	{
		logger.log("Instance created");
	}


	// debug messenger setup
	if constexpr (enableDebugMessages)
	{
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
		debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugCreateInfo.pfnUserCallback = debugCallback;
		debugCreateInfo.pUserData = static_cast<void *>(&logger);

		std::string funcName{"vkCreateDebugUtilsMessengerEXT"};
		auto createFunc = getFunctionPointer<PFN_vkCreateDebugUtilsMessengerEXT>(instance, funcName.c_str());
		if (createFunc(instance, &debugCreateInfo, nullptr, &debugMessenger) != VK_SUCCESS)
		{
			logger.log("Error setting up debug messenger");
		}
	}

}

void VulkanRenderer::prepareShutdown()
{
	// wait for all queues to empty prior to cleaning up
	// validation layers can cause memory leaks without this
	vkDeviceWaitIdle(device);
}

void VulkanRenderer::shutdown()
{
	logger.log("Starting shutdown");
	cleanupSwapchain();

	// command buffers
	vkFreeCommandBuffers(device, commandPool, static_cast<unsigned int>(commandBuffers.size()), commandBuffers.data());
	vkFreeCommandBuffers(device, commandPool, static_cast<unsigned int>(geometryCommandBuffers.size()), geometryCommandBuffers.data());
	vkFreeCommandBuffers(device, commandPool, static_cast<unsigned int>(alphaCommandBuffers.size()), alphaCommandBuffers.data());
	vkFreeCommandBuffers(device, commandPool, static_cast<unsigned int>(deferredCommandBuffers.size()), deferredCommandBuffers.data());
	vkFreeCommandBuffers(device, commandPool, static_cast<unsigned int>(skyboxCommandBuffers.size()), skyboxCommandBuffers.data());
	vkFreeCommandBuffers(device, commandPool, static_cast<unsigned int>(uiCommandBuffers.size()), uiCommandBuffers.data());
	vkFreeCommandBuffers(device, commandPool, static_cast<unsigned int>(debugCommandBuffers.size()), debugCommandBuffers.data());
	logger.log("Destroyed command buffers");

	vkDestroyRenderPass(device, renderPass, nullptr);
	logger.log("Render passes destroyed");

	renderDescriptors.cleanup(device);
	materialDescriptors.cleanup(device);
	deferredDescriptors.cleanup(device);
	logger.log("Destroyed descriptors");

	// Cleanup all resources allocated for various assets
	for (int i = 0; i < textures.getSize(); ++i)
	{
		const TextureImage &ti = textures.getAssets()[i];
		if (textures.isOccupied(i))
		{
			vkFreeMemory(device, ti.memory, nullptr);
			vkDestroyImage(device, ti.image, nullptr);
			vkDestroyImageView(device, ti.imageView, nullptr);
		}
	}

	for (int i = 0; i < primitives.getSize(); ++i)
	{
		const PrimitiveBuffers &pb = primitives.getAssets()[i];
		if (primitives.isOccupied(i))
		{
			freeBuffer(buffers.get(pb.getVertexBufferId()));
			freeBuffer(buffers.get(pb.getIndexBufferId()));
		}
	}

	freeBuffer(buffers.get(matrixBufferId));
	freeBuffer(buffers.get(lightsBufferId));
	freeBuffer(buffers.get(materialBufferId));
	logger.log("Cleaned up buffers and memory");

	// delete the shader module
	vkDestroyShaderModule(device, gBufferModules.vertex, nullptr);
	vkDestroyShaderModule(device, gBufferModules.fragment, nullptr);
	vkDestroyShaderModule(device, skyboxModules.vertex, nullptr);
	vkDestroyShaderModule(device, skyboxModules.fragment, nullptr);
	vkDestroyShaderModule(device, deferredModules.vertex, nullptr);
	vkDestroyShaderModule(device, deferredModules.fragment, nullptr);
	vkDestroyShaderModule(device, uiModules.vertex, nullptr);
	vkDestroyShaderModule(device, uiModules.fragment, nullptr);
	vkDestroyShaderModule(device, glyphModules.vertex, nullptr);
	vkDestroyShaderModule(device, glyphModules.fragment, nullptr);
	vkDestroyShaderModule(device, debugModules.vertex, nullptr);
	vkDestroyShaderModule(device, debugModules.fragment, nullptr);
	logger.log("Destroyed shader modules");
	
	// sync objects cleanup
	for (unsigned int i = 0; i < getMaxFramesInFlight(); ++i)
	{
		vkDestroySemaphore(device, imageSemaphores[i], nullptr);
		vkDestroySemaphore(device, renderSemaphores[i], nullptr);
		vkDestroyFence(device, frameFences[i], nullptr);
	}
	logger.log("Destroed semaphores");

	// command related
	vkDestroyCommandPool(device, commandPool, nullptr);
	logger.log("Destroyed graphics command pool");
	if (transferQueue != graphicsQueue)
	{
		vkDestroyCommandPool(device, transferPool, nullptr);
		logger.log("Destroyed transfer command pool");
	}

	// clean up debug callback
	if constexpr (enableDebugMessages)
	{
		std::string funcName{"vkDestroyDebugUtilsMessengerEXT"};
		auto destroyFunc = getFunctionPointer<PFN_vkDestroyDebugUtilsMessengerEXT>(instance, funcName.c_str());
		destroyFunc(instance, debugMessenger, nullptr);
	}

	textureSampler.destroy(device);
	cubemapSampler.destroy(device);
	glyphAtlasSampler.destroy(device);

	// cleanup Vulkan device and instance
	if (device != VK_NULL_HANDLE)
	{
		vkDestroyDevice(device, nullptr);
		logger.log("Device destroyed");
	}
    if (surface != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(instance, surface, nullptr);
        logger.log("Surface destroyed");
    }
	if (instance != VK_NULL_HANDLE)
	{
		vkDestroyInstance(instance, nullptr);
		logger.log("Instance destroyed");
	}
	cleanedUp = true;
}

VulkanRenderer::~VulkanRenderer()
{
	// in case shutdown is explicitly called
	if (!cleanedUp)
	{
		shutdown();
	}
}

void VulkanRenderer::initialize(const Size &size)
{
	Renderer::initialize(size);
	
	// find compatible GPUs
	physicalDevice = VK_NULL_HANDLE;
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
	if (!deviceCount)
	{
		throw std::runtime_error("No GPUs found");
	}
	else
	{
		std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());

		logger.log("Listing physical devices...");
		for (VkPhysicalDevice device : physicalDevices)
		{
			VkPhysicalDeviceProperties devProps;
			vkGetPhysicalDeviceProperties(device, &devProps);
			logger.log("Physical device: {} with ID: {}", devProps.deviceName, devProps.deviceID);
		}

		for (VkPhysicalDevice device : physicalDevices)
		{
			VkPhysicalDeviceProperties devProps;
			vkGetPhysicalDeviceProperties(device, &devProps);
			VkPhysicalDeviceFeatures devFeats;
			vkGetPhysicalDeviceFeatures(device, &devFeats);

			if (devProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && devFeats.samplerAnisotropy)
			{
				deviceProperties = devProps;
				deviceFeatures = devFeats;
				logger.log("Using: " + std::string(devProps.deviceName));
				physicalDevice = device;
				break;
			}
		}
		if (physicalDevice == VK_NULL_HANDLE)
		{
			throw std::runtime_error("No suitable physical device found");
		}

		// queue family query
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

		if (queueFamilyCount)
		{
			logger.log("Physical device has {} queues", queueFamilyCount);
			std::vector<VkQueueFamilyProperties> queueFamProps(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamProps.data());

			int i = 0;
			for (const auto &queueFamProp : queueFamProps)
			{
				if (queueFamProp.queueCount)
				{
					if (queueFamProp.queueFlags & VK_QUEUE_GRAPHICS_BIT && !queueFamilyIndices.graphics.has_value())
					{
						queueFamilyIndices.graphics = i;
						logger.log("Found graphics queue at index {}", i);
					}
					else if (queueFamProp.queueFlags & VK_QUEUE_TRANSFER_BIT && !queueFamilyIndices.transfer.has_value())
					{
						queueFamilyIndices.transfer = i;
						logger.log("Found transfer queue at index {}", i);
					}
					VkBool32 presentationSupport = false;
					vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentationSupport);
					if (presentationSupport && !queueFamilyIndices.presentation.has_value())
					{
						queueFamilyIndices.presentation = i;
						logger.log("Found presentation queue at index {}", i);
					}
				}
				++i;
			}
		}
		if (!queueFamilyIndices.graphics || !queueFamilyIndices.presentation)
		{
			throw std::runtime_error("Couldn't find required queue family indices");
		}

		// device extensions
		std::vector<const char *> deviceExtensions =
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
		uint32_t deviceExtensionCount = 0;
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, nullptr);
		std::vector<VkExtensionProperties> devExtensionsSupported(deviceExtensionCount);
		vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &deviceExtensionCount, devExtensionsSupported.data());

		std::vector<const char *> requestedDeviceExtensions;
		for (const char *extension : deviceExtensions)
		{
			bool isSupported = false;
			for (const auto &supportedExt : devExtensionsSupported)
			{
				if (strcmp(extension, supportedExt.extensionName) == 0)
				{
					isSupported = true;
					requestedDeviceExtensions.push_back(extension);
					logger.log(std::string(extension) + " device extension will be enabled");
				}
			}
			if (!isSupported)
			{
				throw std::runtime_error("Unsupported device extension: " + std::string(extension));
			}
		}

		// create queues, ensure indices are unique (set)
		if (queueFamilyIndices.graphics.has_value()) uniqueQueueIndices.insert(queueFamilyIndices.graphics.value());
		if (queueFamilyIndices.presentation.has_value()) uniqueQueueIndices.insert(queueFamilyIndices.presentation.value());
		if (queueFamilyIndices.transfer.has_value()) uniqueQueueIndices.insert(queueFamilyIndices.transfer.value());

		logger.log("Creating {} queue(s)", uniqueQueueIndices.size());

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		for (uint32_t index : uniqueQueueIndices)
		{
			const float queuePriority = 1.0f;
			VkDeviceQueueCreateInfo queueCreateInfo = {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = index;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}

		// declare device features
		VkPhysicalDeviceFeatures deviceFeatures = {};
		deviceFeatures.samplerAnisotropy = VK_TRUE;

		// create the logical device
		VkDeviceCreateInfo deviceCreateInfo = {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
		deviceCreateInfo.queueCreateInfoCount = static_cast<unsigned int>(queueCreateInfos.size());
		deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
		deviceCreateInfo.enabledExtensionCount = static_cast<unsigned int>(requestedDeviceExtensions.size());
		deviceCreateInfo.ppEnabledExtensionNames = requestedDeviceExtensions.data();

		if (enableValidationLayers)
		{
			deviceCreateInfo.enabledLayerCount = static_cast<unsigned int>(validationLayers.size());
			deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else
		{
			deviceCreateInfo.enabledLayerCount = 0;
		}
		if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS)
		{
			throw std::runtime_error("Unable to create Vulkan device");
		}
		else
		{
			logger.log("Device created");
		}

		// retrieve queue handles
		if (queueFamilyIndices.graphics.has_value())
		{
			vkGetDeviceQueue(device, queueFamilyIndices.graphics.value(), 0, &graphicsQueue);
			logger.log("Graphics queue found.");
		}
		if (queueFamilyIndices.presentation.has_value())
		{
			vkGetDeviceQueue(device, queueFamilyIndices.presentation.value(), 0, &presentationQueue);
			logger.log("Presentation queue found.");
		}

		if (queueFamilyIndices.transfer.has_value())
		{
			vkGetDeviceQueue(device, queueFamilyIndices.transfer.value(), 0, &transferQueue);
			logger.log("Transfer queue found.");
		}
		else
		{
			logger.log("Transfer queue will be the same as the graphics queue");
			transferQueue = graphicsQueue;
		}

		// shader modules to fill g-buffer
		auto gBuffVertData = FileManager::readBinaryFile("shaders/gbuffer.vert.spv");
		gBufferModules.vertex = createShaderModule(gBuffVertData);
		auto gBuffFragData = FileManager::readBinaryFile("shaders/gbuffer.frag.spv");
		gBufferModules.fragment = createShaderModule(gBuffFragData);
		// skybox rendering shaders
		auto skyboxVertData = FileManager::readBinaryFile("shaders/skybox.vert.spv");
		skyboxModules.vertex = createShaderModule(skyboxVertData);
		auto skyboxFragData = FileManager::readBinaryFile("shaders/skybox.frag.spv");
		skyboxModules.fragment = createShaderModule(skyboxFragData);

		// shader modules to compose final image
		auto deferredVertData = FileManager::readBinaryFile("shaders/deferred.vert.spv");
		deferredModules.vertex = createShaderModule(deferredVertData);
		auto deferredFragData = FileManager::readBinaryFile("shaders/deferred.frag.spv");
		deferredModules.fragment = createShaderModule(deferredFragData);

		// shader modules to compose final image
		auto uiVertData = FileManager::readBinaryFile("shaders/ui.vert.spv");
		uiModules.vertex = createShaderModule(uiVertData);
		auto uiFragData = FileManager::readBinaryFile("shaders/ui.frag.spv");
		uiModules.fragment = createShaderModule(uiFragData);

		// shader modules to compose final image
		auto glyphVertData = FileManager::readBinaryFile("shaders/ui.vert.spv");
		glyphModules.vertex = createShaderModule(glyphVertData);
		auto glyphFragData = FileManager::readBinaryFile("shaders/glyph.frag.spv");
		glyphModules.fragment = createShaderModule(glyphFragData);

		auto debugVertData = FileManager::readBinaryFile("shaders/debug.vert.spv");
		debugModules.vertex = createShaderModule(debugVertData);
		auto debugFragData = FileManager::readBinaryFile("shaders/debug.frag.spv");
		debugModules.fragment = createShaderModule(debugFragData);

		createSwapChain();
		createGBuffers();
		createDepthResources();
		
		renderPass = createRenderPass();
		createFramebuffers();
		createDescriptorSets();
		createGraphicsPipelines();

		const VkDeviceSize matrixBufferSize = MAX_OBJECTS * sizeof(mat4) + sizeof(RenderMatrices) + deviceProperties.limits.minUniformBufferOffsetAlignment;
		matrixBufferId = createBuffer(matrixBufferSize, BufferUsage::UNIFORM, MemoryType::DEVICE_LOCAL_HOST_CACHED);
		const VkDeviceSize lightsBufferSize = sizeof(LightSource) * maxLights;
		lightsBufferId = createBuffer(lightsBufferSize, BufferUsage::UNIFORM, MemoryType::DEVICE_LOCAL_HOST_CACHED);
		const VkDeviceSize matsBufferSize = sizeof(ShaderMaterial) * maxMaterials;
		materialBufferId = createBuffer(matsBufferSize, BufferUsage::UNIFORM, MemoryType::DEVICE_LOCAL_HOST_CACHED);

		commandPool = createCommandPools(queueFamilyIndices, graphicsQueue, transferQueue);
		createCommandBuffers();
		createSynchronization();
		createTextureSamplers();
	}
}

VkShaderModule VulkanRenderer::createShaderModule(const std::vector<char> &contents) const
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = contents.size();
	createInfo.pCode = reinterpret_cast<const uint32_t *>(contents.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("Error creating shader module");
	}
	return shaderModule;
}

void VulkanRenderer::createGBuffers()
{
	gBuffers.resize(swapChainImages.size());

	// using swapchain extent for size
	Size imageSize(swapChainExtent.width, swapChainExtent.height);

	// formats for each g buffer
	positionFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
	albedoFormat = VK_FORMAT_R8G8B8A8_UNORM;
	normalFormat = VK_FORMAT_R16G16B16A16_SFLOAT;

	auto createBuffers = [this, imageSize](OffscreenBuffer &offscreenBuffer, VkFormat imageFormat)
	{
		TextureRequest request(imageSize, imageFormat);
		// TODO: Try using TRANSIENT image usage
		request.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
		auto imagePair = createImage(request);
		offscreenBuffer.image = imagePair.first;
		offscreenBuffer.imageMemory = imagePair.second;
		offscreenBuffer.imageView = createImageView(imagePair.first, imageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	};

	for (size_t i = 0; i < swapChainImages.size(); ++i)
	{
		createBuffers(gBuffers[i].positions, positionFormat);
		createBuffers(gBuffers[i].albedo, albedoFormat);
		createBuffers(gBuffers[i].normals, normalFormat);
	}
}

void VulkanRenderer::createSwapChain()
{
	// swap chain and presentation details
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);

	// surface formats
	std::vector<VkSurfaceFormatKHR> formats;

	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
	if (formatCount > 0)
	{
		formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data());
	}

	// presentation modes
	std::vector<VkPresentModeKHR> presentModes;
	uint32_t presentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
	if (presentModeCount > 0)
	{
		presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, presentModes.data());
	}

	// check if swap chain support is adequate
	if (formats.empty() || presentModes.empty())
	{
		throw std::runtime_error("Swap chain support is incomplete");
	}

	// choose surface format
    VkSurfaceFormatKHR selectedFormat = formats[0];
	for (const auto &format : formats)
	{
        //if (format.format == VK_FORMAT_R8G8B8A8_UNORM &&
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM &&
            format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			selectedFormat = format;
			logger.log("Found preferred surface image format");
		}
	}

	// select the presentation mode
	logger.log("Configuring presentation mode");
	VkPresentModeKHR selectedPresentMode = VK_PRESENT_MODE_FIFO_KHR;
	for (const auto &presentMode : presentModes)
	{
		if (presentMode == preferredPresentMode)
		{
			selectedPresentMode = presentMode;
			logger.log("Found preferred presentation mode");
		}
	}

	// swap chain extent
	VkExtent2D surfaceExtent;
	surfaceExtent.width = std::max(surfaceCapabilities.minImageExtent.width,
									std::min(surfaceCapabilities.maxImageExtent.width, static_cast<uint32_t>(getScreenSize().width)));
	surfaceExtent.height = std::max(surfaceCapabilities.minImageExtent.height,
									std::min(surfaceCapabilities.maxImageExtent.height, static_cast<uint32_t>(getScreenSize().height)));

	uint32_t imageCount = surfaceCapabilities.minImageCount + 1; // extra image to avoid waiting on the driver
	// ensure we don't request more than max available swapchain images
	if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
	{
		imageCount = surfaceCapabilities.maxImageCount;
	}

	// create the swap chain
	VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
	swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapChainCreateInfo.surface = surface;
	swapChainCreateInfo.minImageCount = imageCount;
	swapChainCreateInfo.imageFormat = selectedFormat.format;
	swapChainCreateInfo.imageColorSpace = selectedFormat.colorSpace;
	swapChainCreateInfo.imageExtent = surfaceExtent;
	swapChainCreateInfo.imageArrayLayers = 1;
	swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapChainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
	swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	swapChainCreateInfo.presentMode = selectedPresentMode;
	swapChainCreateInfo.clipped = VK_TRUE;
	swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	// handle swap image ownership in case of different queues
	std::array<uint32_t, 2> queueIndices = { queueFamilyIndices.graphics.value(), queueFamilyIndices.presentation.value() };
	if (graphicsQueue != presentationQueue)
	{
		logger.log("Swapchain image sharing set to CONCURRENT");
		swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapChainCreateInfo.queueFamilyIndexCount = static_cast<unsigned int>(queueIndices.size());
		swapChainCreateInfo.pQueueFamilyIndices = queueIndices.data();
	}
	else
	{
		logger.log("Swapchain image sharing set to EXCLUSIVE");
		swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapChainCreateInfo.queueFamilyIndexCount = 0;
		swapChainCreateInfo.pQueueFamilyIndices = nullptr;
	}

	if (vkCreateSwapchainKHR(device, &swapChainCreateInfo, nullptr, &swapChain) != VK_SUCCESS)
	{
		throw std::runtime_error("Error creating swapchain");
	}
	logger.log("Created swapchain with {} images", imageCount);

	// retrieve the images from the swapchain
	uint32_t swapImagesCount = 0;
	vkGetSwapchainImagesKHR(device, swapChain, &swapImagesCount, nullptr);
	swapChainImages.resize(swapImagesCount);
	vkGetSwapchainImagesKHR(device, swapChain, &swapImagesCount, swapChainImages.data());
	logger.log("Got {} swapchain images", swapImagesCount);

	assert(getMaxFramesInFlight() <= swapImagesCount);

	swapChainFormat = selectedFormat.format;
	swapChainExtent = surfaceExtent;

	// create the image views
	swapChainImageViews.resize(swapChainImages.size());
	for (size_t i = 0; i < swapChainImages.size(); ++i)
	{
		swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainFormat,
												 VK_IMAGE_ASPECT_COLOR_BIT);
	}
}

VkRenderPass VulkanRenderer::createRenderPass()
{
	auto createAttachment = [](VkFormat format, VkImageLayout layout) -> VkAttachmentDescription
	{
		VkAttachmentDescription attachment = {};
		attachment.format = format;
		attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		attachment.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachment.finalLayout = layout;
		return attachment;
	};
	// framebuffer attachments
	VkAttachmentDescription positionAttachment = createAttachment(positionFormat, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	VkAttachmentDescription albedoAttachment = createAttachment(albedoFormat, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	VkAttachmentDescription normalAttachment = createAttachment(normalFormat, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	VkAttachmentDescription colorAttachment = createAttachment(swapChainFormat, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	VkAttachmentDescription depthAttachment = createAttachment(findDepthFormat(), VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;

	// framebuffer attachments
	std::array<VkAttachmentDescription, 5> attachments = {
		colorAttachment, depthAttachment, positionAttachment, albedoAttachment, normalAttachment
	};

	std::array<VkAttachmentReference, 3> colorAttachmentRefs{};
	colorAttachmentRefs[0].attachment = 2; // positions
	colorAttachmentRefs[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	colorAttachmentRefs[1].attachment = 3; // albedo
	colorAttachmentRefs[1].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	colorAttachmentRefs[2].attachment = 4; // normals
	colorAttachmentRefs[2].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	std::array<VkAttachmentReference, 3> inputColorAttachRefs{};
	inputColorAttachRefs[0].attachment = 2; // positions
	inputColorAttachRefs[0].layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	inputColorAttachRefs[1].attachment = 3; // albedo
	inputColorAttachRefs[1].layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	inputColorAttachRefs[2].attachment = 4; // normals
	inputColorAttachRefs[2].layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	// swapchain image
	VkAttachmentReference swapColorAttachRef{};
	swapColorAttachRef.attachment = 0;
	swapColorAttachRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	// depth/stencil image
	VkAttachmentReference depthAttachRef = {};
	depthAttachRef.attachment = 1;
	depthAttachRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	std::array<VkSubpassDescription, 5> subpasses{};
	// gbuffer subpass
	subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpasses[0].colorAttachmentCount = static_cast<unsigned int>(colorAttachmentRefs.size());
	subpasses[0].pColorAttachments = colorAttachmentRefs.data();
	subpasses[0].pDepthStencilAttachment = &depthAttachRef;
	// alpha subpass
	subpasses[1].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpasses[1].colorAttachmentCount = static_cast<unsigned int>(colorAttachmentRefs.size());
	subpasses[1].pColorAttachments = colorAttachmentRefs.data();
	subpasses[1].pDepthStencilAttachment = &depthAttachRef;
	// deferred subpass
	std::array<uint32_t, 1> depthPreserve = {1}; // preserve depth-buffer
	subpasses[2].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpasses[2].colorAttachmentCount = 1;
	subpasses[2].pColorAttachments = &swapColorAttachRef;
	subpasses[2].inputAttachmentCount = static_cast<unsigned int>(inputColorAttachRefs.size());
	subpasses[2].pInputAttachments = inputColorAttachRefs.data();
	subpasses[2].preserveAttachmentCount = depthPreserve.size();
	subpasses[2].pPreserveAttachments = depthPreserve.data();
	// skybox subpass
	subpasses[3].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpasses[3].colorAttachmentCount = 1;
	subpasses[3].pColorAttachments = &swapColorAttachRef;
	subpasses[3].pDepthStencilAttachment = &depthAttachRef;
	// ui subpass
	subpasses[4].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpasses[4].colorAttachmentCount = 1;
	subpasses[4].pColorAttachments = &swapColorAttachRef;

	// subpass dependencies
    std::vector<VkSubpassDependency> dependencies
	{
		{
			.srcSubpass = 0,
			.dstSubpass = 1,
			.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
			.dependencyFlags = 0
		},
		{
			.srcSubpass = 0,
			.dstSubpass = 2,
			.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
			.dependencyFlags = 0
		},
		{
			.srcSubpass = 1,
			.dstSubpass = 2,
			.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
			.dependencyFlags = 0
		},
		{
			.srcSubpass = 2,
			.dstSubpass = 3,
			.srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
			.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
			.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
			.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT,
			.dependencyFlags = 0
		},
		{
			.srcSubpass = 3,
			.dstSubpass = 4,
			.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
			.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT,
			.dependencyFlags = 0
		}
	};

 
	// create the render pass
	VkRenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = attachments.size();
	renderPassCreateInfo.pAttachments = attachments.data();
	renderPassCreateInfo.subpassCount = subpasses.size();
	renderPassCreateInfo.pSubpasses = subpasses.data();
	renderPassCreateInfo.dependencyCount = dependencies.size();
	renderPassCreateInfo.pDependencies = dependencies.data();

	VkRenderPass renderPass = VK_NULL_HANDLE;
	if (vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass) != VK_SUCCESS)
	{
		throw std::runtime_error("Error creating render pass.");
	}

	return renderPass;
}

void VulkanRenderer::createGraphicsPipelines()
{
	std::array<VkDescriptorSetLayout, 2> descriptorLayouts {
		renderDescriptors.getLayout(),
		materialDescriptors.getLayout()
	};

	// geometry pass
    std::array<VkPushConstantRange, 2> pushConsts{};
    pushConsts[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConsts[0].offset = 0;
    pushConsts[0].size = sizeof(RenderConstants);

	VkPipelineLayoutCreateInfo gBuffPipeLayoutCreateInfo{};
	gBuffPipeLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	gBuffPipeLayoutCreateInfo.setLayoutCount = descriptorLayouts.size();
	gBuffPipeLayoutCreateInfo.pSetLayouts = descriptorLayouts.data();
	gBuffPipeLayoutCreateInfo.pushConstantRangeCount = 1;
	gBuffPipeLayoutCreateInfo.pPushConstantRanges = &pushConsts[0];
    gBuffersPipelineLayout = createGraphicsPipelineLayout(gBuffPipeLayoutCreateInfo);
	
	// deferred pass
    pushConsts[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConsts[1].offset = 0;
    pushConsts[1].size = sizeof(GBufferPushConstants);
	
	VkPipelineLayoutCreateInfo defPipeLayoutCreateInfo{};
	defPipeLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	defPipeLayoutCreateInfo.setLayoutCount = 1;
	defPipeLayoutCreateInfo.pSetLayouts = &deferredDescriptors.getLayout();
	defPipeLayoutCreateInfo.pushConstantRangeCount = 1;
	defPipeLayoutCreateInfo.pPushConstantRanges = &pushConsts[1];
	deferredPipelineLayout = createGraphicsPipelineLayout(defPipeLayoutCreateInfo);

	// UI pass
	VkPipelineLayoutCreateInfo uiPipeLayoutCreateInfo{};
	uiPipeLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	uiPipeLayoutCreateInfo.setLayoutCount = descriptorLayouts.size();
	uiPipeLayoutCreateInfo.pSetLayouts = descriptorLayouts.data();
	uiPipeLayoutCreateInfo.pushConstantRangeCount = 1;
	uiPipeLayoutCreateInfo.pPushConstantRanges = &pushConsts[0];
    uiPipelineLayout = createGraphicsPipelineLayout(uiPipeLayoutCreateInfo);

	// Vertex input stage
	VkVertexInputBindingDescription standardInputBind = {};
	standardInputBind.binding = 0;
	standardInputBind.stride = sizeof(Vertex);
	standardInputBind.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	std::vector<VkVertexInputAttributeDescription> standardAttrDesc;
	standardAttrDesc.push_back({
		.location = 0,
		.binding = 0,
		.format = VK_FORMAT_R32G32B32_SFLOAT,
		.offset = offsetof(Vertex, position)
	});
	standardAttrDesc.push_back({
		.location = 1,
		.binding = 0,
		.format = VK_FORMAT_R32G32B32A32_SFLOAT,
		.offset = offsetof(Vertex, colour)
	});
	standardAttrDesc.push_back({
		.location = 2,
		.binding = 0,
		.format = VK_FORMAT_R32G32_SFLOAT,
		.offset = offsetof(Vertex, textureCoordinates)
	});
	standardAttrDesc.push_back({
		.location = 3,
		.binding = 0,
		.format = VK_FORMAT_R32G32B32_SFLOAT,
		.offset = offsetof(Vertex, normal)
	});

	// pipeline for g-buffer
	gBufferPipeline = GraphicsPipelineBuilder<3>(device, gBuffersPipelineLayout)
		.assembly(&standardInputBind, &standardAttrDesc, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
		.viewport(true, swapChainExtent)
		.rasterizer(VK_CULL_MODE_BACK_BIT)
		.multisampling()
		.blending({AttachmentBlendInfo { .enabled = false }, { .enabled = false }, { .enabled = false }})
		.depth(true, VK_COMPARE_OP_LESS)
		.shaderModules(gBufferModules, true)
		.renderPass(renderPass, 0)
		.build();

	gBufferNoTexPipeline = GraphicsPipelineBuilder<3>(device, gBuffersPipelineLayout)
		.assembly(&standardInputBind, &standardAttrDesc, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
		.viewport(true, swapChainExtent)
		.rasterizer(VK_CULL_MODE_BACK_BIT)
		.multisampling()
		.blending({AttachmentBlendInfo { .enabled = false }, { .enabled = true }, { .enabled = false }})
		.depth(true, VK_COMPARE_OP_LESS)
		.shaderModules(gBufferModules, false)
		.renderPass(renderPass, 0)
		.build();

	alphaBufferPipeline = GraphicsPipelineBuilder<3>(device, gBuffersPipelineLayout)
		.assembly(&standardInputBind, &standardAttrDesc, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
		.viewport(true, swapChainExtent)
		.rasterizer(VK_CULL_MODE_BACK_BIT)
		.multisampling()
		.blending({AttachmentBlendInfo { .enabled = false }, { .enabled = true }, { .enabled = false }})
		.depth(true, VK_COMPARE_OP_LESS)
		.shaderModules(gBufferModules, true)
		.renderPass(renderPass, 1)
		.build();

	// pipeline for deferred final output
	deferredPipeline = GraphicsPipelineBuilder<1>(device, deferredPipelineLayout)
		.assembly(nullptr, nullptr, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
		.viewport(false, swapChainExtent)
		.rasterizer(VK_CULL_MODE_FRONT_BIT)
		.multisampling()
		.blending({AttachmentBlendInfo { .enabled = false }})
		.depth(false, VK_COMPARE_OP_LESS)
		.shaderModules(deferredModules, false)
		.renderPass(renderPass, 2)
		.build();

	// skybox pipeline
	skyboxPipeline = GraphicsPipelineBuilder<1>(device, gBuffersPipelineLayout)
		.assembly(&standardInputBind, &standardAttrDesc, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
		.viewport(true, swapChainExtent)
		.rasterizer(VK_CULL_MODE_BACK_BIT)
		.multisampling()
		.blending({AttachmentBlendInfo { .enabled = false }})
		.depth(true, VK_COMPARE_OP_LESS_OR_EQUAL)
		.shaderModules(skyboxModules, true)
		.renderPass(renderPass, 3)
		.build();

	// debugging-line drawing pipeline
	debugLinePipeline = GraphicsPipelineBuilder<1>(device, uiPipelineLayout)
		.assembly(&standardInputBind, &standardAttrDesc, VK_PRIMITIVE_TOPOLOGY_LINE_LIST)
		.viewport(true, swapChainExtent)
		.rasterizer(VK_CULL_MODE_NONE)
		.multisampling()
		.blending({AttachmentBlendInfo { .enabled = true }})
		.depth(false, VK_COMPARE_OP_LESS)
		.shaderModules(debugModules, false)
		.renderPass(renderPass, 4)
		.build();

	// UI pipeline
	std::array<VkDynamicState, 1> dynamicStates{ VK_DYNAMIC_STATE_SCISSOR };
	uiPipeline = GraphicsPipelineBuilder<1>(device, uiPipelineLayout)
		.assembly(&standardInputBind, &standardAttrDesc, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
		.viewport(true, swapChainExtent)
		.rasterizer(VK_CULL_MODE_NONE)
		.multisampling()
		.blending({AttachmentBlendInfo { .enabled = true }})
		.depth(false, VK_COMPARE_OP_LESS_OR_EQUAL)
		.shaderModules(uiModules, true)
		.renderPass(renderPass, 4)
		.dynamicState(&dynamicStates)
		.build();

	glyphPipeline = GraphicsPipelineBuilder<1>(device, uiPipelineLayout)
		.assembly(&standardInputBind, &standardAttrDesc, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)
		.viewport(true, swapChainExtent)
		.rasterizer(VK_CULL_MODE_NONE)
		.multisampling()
		.blending({AttachmentBlendInfo { .enabled = true }})
		.depth(false, VK_COMPARE_OP_LESS_OR_EQUAL)
		.shaderModules(glyphModules, true)
		.renderPass(renderPass, 4)
		.build();
}

VkPipelineLayout VulkanRenderer::createGraphicsPipelineLayout(const VkPipelineLayoutCreateInfo &createInfo) const
{
	VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
	if (vkCreatePipelineLayout(device, &createInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Error creating pipeline layout");
	}

	return pipelineLayout;
}

void VulkanRenderer::createFramebuffers()
{
	// swapchain framebuffers
	framebuffers.resize(swapChainImageViews.size());
	for (size_t i = 0; i < swapChainImages.size(); ++i)
	{
        std::array<VkImageView, 5> attachments = {
			swapChainImageViews[i],
			depthImages[i].imageView,
			gBuffers[i].positions.imageView,
			gBuffers[i].albedo.imageView,
			gBuffers[i].normals.imageView
		};
			
		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = attachments.size();
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = swapChainExtent.width;
		framebufferInfo.height = swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Error creating framebuffer");
		}
	}

	logger.log("Created framebuffers");
}

void VulkanRenderer::createDescriptorSets()
{
	const int frames = getMaxFramesInFlight();
	// render pass - per frame
	renderDescriptors.setMaxSets(frames);
	std::array<VkDescriptorSetLayoutBinding, 3> renderBindings{};
	renderBindings[0].binding = 0; // general info
	renderBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	renderBindings[0].descriptorCount = 1;
	renderBindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	renderBindings[0].pImmutableSamplers = nullptr;
	renderBindings[1].binding = 1; // matrices
	renderBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	renderBindings[1].descriptorCount = 1;
	renderBindings[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	renderBindings[1].pImmutableSamplers = nullptr;
	renderBindings[2].binding = 2; // materials
	renderBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	renderBindings[2].descriptorCount = 1;
	renderBindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	renderBindings[2].pImmutableSamplers = nullptr;

	std::array<VkDescriptorPoolSize, 3> renderPoolSizes =
	{
		VkDescriptorPoolSize { .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = renderBindings[0].descriptorCount * renderDescriptors.getMaxSets() },
		VkDescriptorPoolSize { .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = renderBindings[1].descriptorCount * renderDescriptors.getMaxSets() },
		VkDescriptorPoolSize { .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, .descriptorCount = renderBindings[2].descriptorCount * renderDescriptors.getMaxSets() }
	};

	renderDescriptors.createLayout(device, renderBindings);
	renderDescriptors.createPool(device, renderPoolSizes);
	renderDescriptors.allocate(device);

	// render pass - per material group
	materialDescriptors.setMaxSets(maxMaterials * frames);
	std::array<VkDescriptorSetLayoutBinding, 1> materialBindings;
	materialBindings[0].binding = 0; // samplers
	materialBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	materialBindings[0].descriptorCount = maxSamplers;
	materialBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	materialBindings[0].pImmutableSamplers = nullptr;

	std::array<VkDescriptorPoolSize, 1> materialPoolSizes{};
	materialPoolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	materialPoolSizes[0].descriptorCount = maxMaterials * frames * maxSamplers;

	materialDescriptors.createLayout(device, materialBindings);
	materialDescriptors.createPool(device, materialPoolSizes);
	materialDescriptors.allocate(device);
	
	// deferred render pass
	std::array<VkDescriptorSetLayoutBinding, 4> deferredBindings{};
	deferredBindings[0].binding = 0; // position
	deferredBindings[0].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	deferredBindings[0].descriptorCount = 1;
	deferredBindings[0].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	deferredBindings[1].binding = 1; // albedos
	deferredBindings[1].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	deferredBindings[1].descriptorCount = 1;
	deferredBindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	deferredBindings[2].binding = 2; // normals
	deferredBindings[2].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	deferredBindings[2].descriptorCount = 1;
	deferredBindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	deferredBindings[3].binding = 3; // lights
	deferredBindings[3].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	deferredBindings[3].descriptorCount = 1;
	deferredBindings[3].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	deferredBindings[3].pImmutableSamplers = nullptr;

	std::array<VkDescriptorPoolSize, 4> attachPoolSizes{};
	attachPoolSizes[0].type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	attachPoolSizes[0].descriptorCount = frames;
	attachPoolSizes[1].type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	attachPoolSizes[1].descriptorCount = frames;
	attachPoolSizes[2].type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	attachPoolSizes[2].descriptorCount = frames;
	attachPoolSizes[3].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	attachPoolSizes[3].descriptorCount = frames;

	deferredDescriptors.setMaxSets(frames);
	deferredDescriptors.createLayout(device, deferredBindings);
	deferredDescriptors.createPool(device, attachPoolSizes);
	deferredDescriptors.allocate(device);
}

VkCommandPool VulkanRenderer::createCommandPools(const QueueFamilyIndices &queueFamilyIndices, const VkQueue &graphicsQueue, const VkQueue &transferQueue)
{
	VkCommandPool commandPool = VK_NULL_HANDLE;
	
	auto createPool = [this](uint32_t index, VkCommandPool *pool) {
		VkCommandPoolCreateInfo commandPoolInfo = {};
		commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolInfo.queueFamilyIndex = index;
		commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		if (vkCreateCommandPool(device, &commandPoolInfo, nullptr, pool) != VK_SUCCESS)
		{
			throw std::runtime_error("Error creating command pool");
		}
	};

	createPool(queueFamilyIndices.graphics.value(), &commandPool);
	if (transferQueue != graphicsQueue)
	{
		createPool(queueFamilyIndices.transfer.value(), &transferPool);
		logger.log("Created dedicated transfer command pool");
	}
	else
	{
		transferPool = commandPool;
		logger.log("Transfer pool will be the same as the graphics pool");
	}

	VkCommandPoolCreateInfo commandPoolInfo = {};
	commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolInfo.queueFamilyIndex = queueFamilyIndices.graphics.value();
	commandPoolInfo.flags = 0;

	return commandPool;
}

void VulkanRenderer::createCommandBuffers()
{
	commandBuffers.resize(getMaxFramesInFlight());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = static_cast<uint32_t>(getMaxFramesInFlight());

	if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Error allocating command buffers");
	}

	logger.log("Allocated {} command buffers", commandBuffers.size());

	createSecondaryCommandBuffers(geometryCommandBuffers);
	createSecondaryCommandBuffers(alphaCommandBuffers);
	createSecondaryCommandBuffers(deferredCommandBuffers);
	createSecondaryCommandBuffers(skyboxCommandBuffers);
	createSecondaryCommandBuffers(uiCommandBuffers);
	createSecondaryCommandBuffers(debugCommandBuffers);
}

void VulkanRenderer::createSecondaryCommandBuffers(std::vector<VkCommandBuffer> &secondaryBuffers)
{
	secondaryBuffers.resize(getMaxFramesInFlight());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = static_cast<uint32_t>(getMaxFramesInFlight());

	if (vkAllocateCommandBuffers(device, &allocInfo, secondaryBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Error allocating secondary command buffers");
	}
	logger.log("Allocated {} secondary command buffers", secondaryBuffers.size());
}

void VulkanRenderer::createSynchronization()
{
	imageSemaphores.resize(getMaxFramesInFlight());
	renderSemaphores.resize(getMaxFramesInFlight());
	frameFences.resize(getMaxFramesInFlight());

	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (unsigned int i = 0; i < getMaxFramesInFlight(); ++i)
	{
		if (vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &imageSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &renderSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(device, &fenceCreateInfo, nullptr, &frameFences[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Unable to create synchronization objects");
		}
	}
}

void VulkanRenderer::createTextureSamplers()
{
	textureSampler = Sampler::create(device, VK_SAMPLER_ADDRESS_MODE_REPEAT);
	cubemapSampler = Sampler::create(device, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE);
	glyphAtlasSampler = Sampler::create(device, VK_SAMPLER_ADDRESS_MODE_REPEAT);
}

void VulkanRenderer::createDepthResources()
{
	VkFormat depthFormat = findDepthFormat();

	depthImages.resize(swapChainImages.size());
	for (unsigned int i = 0; i < swapChainImages.size(); ++i)
	{
		TextureRequest request(Size(static_cast<cgfloat>(swapChainExtent.width), static_cast<cgfloat>(swapChainExtent.height)), depthFormat);
		request.tiling = VK_IMAGE_TILING_OPTIMAL;
		request.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		request.memProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		auto imagePair = createImage(request);

		depthImages[i].image = imagePair.first;
		depthImages[i].memory = imagePair.second;
		depthImages[i].imageView = createImageView(imagePair.first, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
	}
}

void VulkanRenderer::recreateSwapchain()
{
	vkDeviceWaitIdle(device);

	// recreate swapchain
	cleanupSwapchain();
	createSwapChain();
	createGBuffers();
	createCommandBuffers();

	// recreate pipelines due to swapchain changes
	createGraphicsPipelines();

	// depth and framebuffers need to be recreated due to new swapchain
	createDepthResources();
	createFramebuffers();
}

uint32_t VulkanRenderer::findMemoryType(uint32_t memoryTypeBits,
										VkMemoryPropertyFlags propertFlags) const
{
	// memoryTypeBits is a bitset of memoryTypes supported by the resource
	VkPhysicalDeviceMemoryProperties memProps;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProps);

	for (uint32_t i = 0; i < memProps.memoryTypeCount; ++i)
	{
		if (memoryTypeBits & (1 << i) && (memProps.memoryTypes[i].propertyFlags & propertFlags) == propertFlags)
		{
			return i;
		}
	}
	
	throw std::runtime_error("Unable to find memory for buffer");
}

void VulkanRenderer::cleanupSwapchain()
{
	// cleanup resources
	for (const auto &tex : depthImages)
	{
		vkDestroyImage(device, tex.image, nullptr);
		vkFreeMemory(device, tex.memory, nullptr);
		vkDestroyImageView(device, tex.imageView, nullptr);
	}
	logger.log("Cleaned up depth resources");

	for (const auto &framebuffer : framebuffers)
	{
		vkDestroyFramebuffer(device, framebuffer, nullptr);
	}
	logger.log("Destroyed framebuffers");

	for (const auto &imageView : swapChainImageViews)
	{
		vkDestroyImageView(device, imageView, nullptr);
	}
	logger.log("Destroyed image views");

	vkDestroySwapchainKHR(device, swapChain, nullptr);
	logger.log("Swapchain destroyed");

	// clean up graphics pipeline
	GraphicsPipeline::destroy(device, gBufferPipeline);
	GraphicsPipeline::destroy(device, gBufferNoTexPipeline);
	GraphicsPipeline::destroy(device, alphaBufferPipeline);
	GraphicsPipeline::destroy(device, skyboxPipeline);
	GraphicsPipeline::destroy(device, deferredPipeline);
	GraphicsPipeline::destroy(device, uiPipeline);
	GraphicsPipeline::destroy(device, glyphPipeline);
	GraphicsPipeline::destroy(device, debugLinePipeline);

	vkDestroyPipelineLayout(device, gBuffersPipelineLayout, nullptr);
	vkDestroyPipelineLayout(device, deferredPipelineLayout, nullptr);
	vkDestroyPipelineLayout(device, uiPipelineLayout, nullptr);
	logger.log("Pipeline layouts destroyed");

	// clean up g buffers
	for (size_t i = 0; i < swapChainImages.size(); ++i)
	{
		auto destroyGBuffer = [this](const OffscreenBuffer offscreenBuffer)
		{
			vkDestroyImageView(device, offscreenBuffer.imageView, nullptr);
			vkFreeMemory(device, offscreenBuffer.imageMemory, nullptr);
			vkDestroyImage(device, offscreenBuffer.image, nullptr);
		};
		destroyGBuffer(gBuffers[i].positions);
		destroyGBuffer(gBuffers[i].albedo);
		destroyGBuffer(gBuffers[i].normals);
	}
	logger.log("Destroyed g-buffers");
}

void VulkanRenderer::resize(const Boiler::Size &size)
{
	Renderer::resize(size);
	logger.log("Resizing to {}x{}", size.width, size.height);
	resizeOccured = true;
}

std::string VulkanRenderer::getVersion() const
{
	return "Boiler Vulkan 1.0";
}

std::pair<VkImage, VkDeviceMemory> VulkanRenderer::createImage(const TextureRequest &request) const
{
	VkImage textureImage;
	VkDeviceMemory textureMemory;

	VkImageCreateInfo imageInfo = request.createInfo();
	if (vkCreateImage(device, &imageInfo, nullptr, &textureImage) != VK_SUCCESS)
	{
		throw std::runtime_error("Error creating image");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, textureImage, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, request.memProperties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &textureMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("Error allocating image memory");
	}

	vkBindImageMemory(device, textureImage, textureMemory, 0);

	return std::make_pair(textureImage, textureMemory);
}

VkImageView VulkanRenderer::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags,
											VkImageViewType viewType) const
{
	VkImageViewCreateInfo imageViewCreateInfo = {};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.image = image;
	imageViewCreateInfo.viewType = viewType;
	imageViewCreateInfo.format = format;
	imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.subresourceRange.aspectMask = aspectFlags;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = viewType == VK_IMAGE_VIEW_TYPE_CUBE ? 6 : 1;

	VkImageView imageView;
	if (vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageView) != VK_SUCCESS)
	{
		throw std::runtime_error("Unable to create swapchain image view");
	}

	return imageView;
}

constexpr size_t cubeMapSize = 6;
AssetId VulkanRenderer::loadCubemap(const std::array<ImageData, cubeMapSize> &images)
{
	size_t totalSize = 0;

	for (const ImageData &imageData : images)
	{
		totalSize += static_cast<unsigned int>(imageData.size.width) *
			static_cast<unsigned int>(imageData.size.height) *
			static_cast<unsigned int>(imageData.colorComponents);
	}

	// create a single image with 6 layers
	AssetId stageBufferId = createBuffer(totalSize, BufferUsage::STAGING, MemoryType::HOST_CACHED);

	// stage data for all 6 images
	const BufferInfo buffInfo = buffers.get(stageBufferId);
	char *data = nullptr;
	vkMapMemory(device, buffInfo.memory, 0, totalSize, 0, (void **)&data);
	size_t offset = 0;
	for (auto &image : images)
	{
		const size_t size = image.size.width * image.size.height * image.colorComponents;
		memcpy(data + offset, image.pixelData, size);
		offset += size;
	}
	vkUnmapMemory(device, buffInfo.memory);

	// create a multi-layer texture
	TextureRequest request(images[0].size, textureFormat);
	request.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	request.layers = cubeMapSize;
	request.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
	auto imagePair = createImage(request);

	transitionImageLayout(imagePair.first, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 0, 6);
	copyBufferToImage(buffInfo.buffer, 0, imagePair.first, images[0].size, 0, 6);
	freeBuffer(buffInfo);
	transitionImageLayout(imagePair.first, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 0, 6);

	VkImageView imageView = createImageView(imagePair.first, textureFormat, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_CUBE);
	return textures.add(TextureImage(imagePair.first, imagePair.second, imageView));
}

AssetId VulkanRenderer::loadTexture(const ImageData &imageData, TextureType type)
{
	VkFormat format = textureFormat;
	if (type == TextureType::FREETYPE_ATLAS)
	{
		format = VK_FORMAT_R8_UNORM;
	}
	else if (type == TextureType::RGBA_UNORM)
	{
		format = VK_FORMAT_R8G8B8A8_UNORM;
	}
	const size_t bytesPerPixel = imageData.colorComponents;

	// calculate size of buffer and generate the staging buffer
	const VkDeviceSize bytesSize = imageData.size.width * imageData.size.height * bytesPerPixel;

	AssetId stagingBufferId = createBuffer(bytesSize, BufferUsage::STAGING, MemoryType::HOST_CACHED);
	const BufferInfo bufferInfo = buffers.get(stagingBufferId);

	logger.log("Staging {} bytes of data", bytesSize);
	void *data = nullptr;
	vkMapMemory(device, bufferInfo.memory, 0, static_cast<size_t>(bytesSize), 0, &data);
	memcpy(data, imageData.pixelData, static_cast<size_t>(bytesSize));
	vkUnmapMemory(device, bufferInfo.memory);

	TextureRequest request(imageData.size, format);
	request.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	auto imagePair = createImage(request);

	// transition the image to transfer layout, copy the buffer pixel data to the image
	transitionImageLayout(imagePair.first, VK_IMAGE_LAYOUT_UNDEFINED,
						  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(bufferInfo.buffer, 0, imagePair.first, imageData.size);
	
	// cleanup buffer and memory
	freeBuffer(bufferInfo);

	// transition the image to a layout optimal for shader sampling
	transitionImageLayout(imagePair.first, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	VkImageView imageView = createImageView(imagePair.first, format, VK_IMAGE_ASPECT_COLOR_BIT);

	logger.log("Texture data loaded");
	return textures.add(TextureImage(imagePair.first, imagePair.second, imageView));
}

void allocateGlyphs(const GlyphMap &map)
{
}

AssetId VulkanRenderer::createBuffer(size_t size, BufferUsage usage, MemoryType memType)
{
	VkBufferUsageFlags usageFlags;
	VkMemoryPropertyFlags memoryProperties;

	if (usage == BufferUsage::STAGING)
	{
		usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	}
	else if (usage == BufferUsage::VERTICES)
	{
		usageFlags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	}
	else if (usage == BufferUsage::INDICES)
	{
		usageFlags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	}
	else if (usage == BufferUsage::UNIFORM)
	{
		usageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	}

	if (memType == MemoryType::DEVICE_LOCAL)
	{
		memoryProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	}
	else if (memType == MemoryType::DEVICE_LOCAL_HOST_CACHED)
	{
		memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	}
	else if (memType == MemoryType::HOST_CACHED)
	{
		memoryProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
	}

	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usageFlags;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkBuffer buffer;
	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Error creating buffer");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	// allocate memory for the buffer
	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, memoryProperties);

	VkDeviceMemory bufferMemory;
	if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("Unable to allocate buffer memory");
	}
	// bind the memory to the buffer and map it
	vkBindBufferMemory(device, buffer, bufferMemory, 0);

	return buffers.add(BufferInfo(buffer, bufferMemory, size));
}

VkCommandBuffer VulkanRenderer::beginSingleTimeCommands(const VkCommandPool &pool) const
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = pool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	if (vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Unable to allocate command buffer");
	}

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	if(vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("Unable to begin allocate command buffer");
	}

	return commandBuffer;
}

void VulkanRenderer::endSingleTimeCommands(const VkQueue &queue, const VkCommandPool &pool, const VkCommandBuffer &buffer) const
{
	vkEndCommandBuffer(buffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &buffer;

	// TODO: Handle multi operations
	vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(queue);

	vkFreeCommandBuffers(device, pool, 1, &buffer);
}

void VulkanRenderer::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize dataSize) const
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands(transferPool);
	
	VkBufferCopy copyRegion = {};
	copyRegion.size = dataSize;
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	endSingleTimeCommands(transferQueue, transferPool, commandBuffer);
}

void VulkanRenderer::transitionImageLayout(VkImage image, VkImageLayout oldLayout, VkImageLayout newLayout,
										   unsigned int arrayLayer, unsigned int layerCount) const
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands(commandPool);

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = arrayLayer;
	barrier.subresourceRange.layerCount = layerCount;

	// configure pipeline stages and access
	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;
	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED &&
		newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
			 newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
	{
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else
	{
		throw std::runtime_error("Unsupported layout transition");
	}

	vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0,
						 nullptr, 0, nullptr, 1, &barrier);

	endSingleTimeCommands(graphicsQueue, commandPool, commandBuffer);
}

void VulkanRenderer::copyBufferToImage(VkBuffer buffer, size_t bufferOffset, VkImage image, const Size &imageSize,
									   unsigned int arrayLayer, unsigned int layerCount) const
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands(transferPool);

	VkBufferImageCopy region = {};
	region.bufferOffset = bufferOffset;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = arrayLayer;
	region.imageSubresource.layerCount = layerCount;

	region.imageOffset = {0, 0, 0};
	region.imageExtent = {static_cast<uint32_t>(imageSize.width), static_cast<uint32_t>(imageSize.height), 1};

	vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	endSingleTimeCommands(transferQueue, transferPool, commandBuffer);
}

VkFormat VulkanRenderer::findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling,
											 VkFormatFeatureFlags features) const
{
	for (VkFormat format : candidates)
	{
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
		{
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
		{
			return format;
		}
	}
	throw std::runtime_error("Unable to find supported format");
}

VkFormat VulkanRenderer::findDepthFormat() const
{
	return findSupportedFormat({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
		VK_IMAGE_TILING_OPTIMAL, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

bool VulkanRenderer::hasStencilComponent(VkFormat format) const
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

AssetId VulkanRenderer::createGPUBuffer(void *data, size_t size, BufferUsage usage)
{
	VkDeviceSize bufferSize = size;
	// create a staging buffer (host), map memory and copy
	AssetId stageBufferId = createBuffer(bufferSize, BufferUsage::STAGING, MemoryType::HOST_CACHED);
	const BufferInfo &stageBufferInfo = buffers.get(stageBufferId);

	void *mappedMem = nullptr;
	vkMapMemory(device, stageBufferInfo.memory, 0, bufferSize, 0, &mappedMem);
	memcpy(mappedMem, data, bufferSize);
	vkUnmapMemory(device, stageBufferInfo.memory);

	// create a device-local buffer
	AssetId bufferId = createBuffer(bufferSize, usage, MemoryType::DEVICE_LOCAL);
	const BufferInfo &bufferInfo = buffers.get(bufferId);

	copyBuffer(stageBufferInfo.buffer, bufferInfo.buffer, bufferInfo.size);

	buffers.remove(stageBufferId);
	freeBuffer(stageBufferInfo);

	return bufferId;
}

AssetId VulkanRenderer::loadPrimitive(const VertexData &data, AssetId existingId)
{
	AssetId vertexBufferId = createGPUBuffer((void *)data.vertexBegin(), data.vertexByteSize(), BufferUsage::VERTICES);
	AssetId indexBufferId = createGPUBuffer((void *)data.indexBegin(), data.indexByteSize(), BufferUsage::INDICES);

	AssetId primBuffId = existingId;
	if (primBuffId == Asset::NO_ASSET)
	{
		primBuffId = primitives.add(PrimitiveBuffers(vertexBufferId, indexBufferId));
	}
	else
	{
		// delete existing buffers and set the new ones in their place
		PrimitiveBuffers &primitiveBuffers = primitives.get(primBuffId);
		freeBuffer(buffers.get(primitiveBuffers.getVertexBufferId()));
		freeBuffer(buffers.get(primitiveBuffers.getIndexBufferId()));

		buffers.remove(primitiveBuffers.getVertexBufferId());
		buffers.remove(primitiveBuffers.getIndexBufferId());

		primitiveBuffers.setBuffers(vertexBufferId, indexBufferId);
	}

	return primBuffId;
}

void VulkanRenderer::updateLights(const std::vector<LightSource> &lightSources) const
{
	// copy light source data
	const BufferInfo lightsBuffer = buffers.get(lightsBufferId);

	void *lightData = nullptr;
	vkMapMemory(device, lightsBuffer.memory, 0, lightsBuffer.size, 0, &lightData);
	memcpy(lightData, lightSources.data(), lightsBuffer.size);
	vkUnmapMemory(device, lightsBuffer.memory);
}

void VulkanRenderer::updateMaterials(const std::vector<ShaderMaterial> &materials) const
{
	assert(materials.size() <= maxMaterials);
	const BufferInfo materialBuffer = buffers.get(materialBufferId);

	void *data = nullptr;
	vkMapMemory(device, materialBuffer.memory, 0, materialBuffer.size, 0, &data);
	memcpy(data, materials.data(), materialBuffer.size);
	vkUnmapMemory(device, materialBuffer.memory);
}

void VulkanRenderer::freeBuffer(const BufferInfo &bufferInfo) const
{
	vkDestroyBuffer(device, bufferInfo.buffer, nullptr);
	vkFreeMemory(device, bufferInfo.memory, nullptr);
}

template<class T>
void updateMemory(VkDevice device, VkDeviceMemory memory, const T &object)
{
	void *data = nullptr;
	vkMapMemory(device, memory, 0, sizeof(T), 0, &data);
	memcpy(data, &object, sizeof(T));
	vkUnmapMemory(device, memory);
}

void beginSecondaryCommandBuffer(VkCommandBuffer secondaryBuffer, VkFramebuffer frameBuffer, VkRenderPass renderPass, uint32_t subpassIndex)
{
	VkCommandBufferInheritanceInfo inheritInfo = {};
	inheritInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
	inheritInfo.renderPass = renderPass;
	inheritInfo.subpass = subpassIndex;
	inheritInfo.framebuffer = frameBuffer;

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
	beginInfo.pInheritanceInfo = &inheritInfo;

	if (vkBeginCommandBuffer(secondaryBuffer, &beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("Could not begin secondary command buffer");
	}
}

bool VulkanRenderer::prepareFrame(const FrameInfo &frameInfo)
{
	bool shouldRender = Renderer::prepareFrame(frameInfo);

	vkWaitForFences(device, 1, &frameFences[frameInfo.currentFrame], VK_TRUE, UINT32_MAX);
	vkResetFences(device, 1, &frameFences[frameInfo.currentFrame]);

	nextImageResult = vkAcquireNextImageKHR(device, swapChain, UINT32_MAX, imageSemaphores[frameInfo.currentFrame],
											VK_NULL_HANDLE, &imageIndex);

    if (nextImageResult == VK_SUCCESS || nextImageResult == VK_SUBOPTIMAL_KHR
            && shouldRender && !resizeOccured)
	{
		// setup the command buffers
		const VkCommandBuffer commandBuffer = commandBuffers[frameInfo.currentFrame];
		vkResetCommandBuffer(commandBuffers[frameInfo.currentFrame], 0);
		vkResetCommandBuffer(geometryCommandBuffers[frameInfo.currentFrame], 0);
		vkResetCommandBuffer(alphaCommandBuffers[frameInfo.currentFrame], 0);
		vkResetCommandBuffer(deferredCommandBuffers[frameInfo.currentFrame], 0);
		vkResetCommandBuffer(skyboxCommandBuffers[frameInfo.currentFrame], 0);
		vkResetCommandBuffer(uiCommandBuffers[frameInfo.currentFrame], 0);
		vkResetCommandBuffer(debugCommandBuffers[frameInfo.currentFrame], 0);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Could not begin command buffer");
		}

		// clear colour
		std::array<VkClearValue, 5> clearValues = {};
        clearValues[0].color = {{getClearColor().r, getClearColor().g, getClearColor().b, 1.0f}};
		clearValues[1].depthStencil = {1.0f, 0};
        clearValues[2].color = {{0, 0, 0, 0}};
        clearValues[3].color = {{1, 1, 1, 1}};
        clearValues[4].color = {{0, 0, 0, 0}};

		// begin the render pass
		VkRenderPassBeginInfo renderBeginInfo = {};
		renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderBeginInfo.renderPass = renderPass;
		renderBeginInfo.framebuffer = framebuffers[imageIndex];
		renderBeginInfo.renderArea.offset = {0, 0};
		renderBeginInfo.renderArea.extent = swapChainExtent;
		renderBeginInfo.clearValueCount = clearValues.size();
		renderBeginInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffer, &renderBeginInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

		// begin the secondary command buffers for various subpasses
		beginSecondaryCommandBuffer(geometryCommandBuffers[frameInfo.currentFrame], framebuffers[imageIndex], renderPass, 0);
		beginSecondaryCommandBuffer(alphaCommandBuffers[frameInfo.currentFrame], framebuffers[imageIndex], renderPass, 1);
		beginSecondaryCommandBuffer(deferredCommandBuffers[frameInfo.currentFrame], framebuffers[imageIndex], renderPass, 2);
		beginSecondaryCommandBuffer(skyboxCommandBuffers[frameInfo.currentFrame], framebuffers[imageIndex], renderPass, 3);
		beginSecondaryCommandBuffer(uiCommandBuffers[frameInfo.currentFrame], framebuffers[imageIndex], renderPass, 4);
		beginSecondaryCommandBuffer(debugCommandBuffers[frameInfo.currentFrame], framebuffers[imageIndex], renderPass, 4);

		// update the descriptor sets for this frame / buffers are updated later
		static std::array<VkDescriptorSet, 2> descriptorSets{};
		descriptorSets[DSET_IDX_FRAME] = renderDescriptors.getSet(frameInfo.currentFrame);

		std::array<VkWriteDescriptorSet, 3> dsetWritesFrame{};

		// view and projection matrices
		const BufferInfo &matrixBuffer = buffers.get(matrixBufferId);
		VkDescriptorBufferInfo viewProjBuffInfo = {};
		viewProjBuffInfo.buffer = matrixBuffer.buffer;
		viewProjBuffInfo.offset = 0;
		viewProjBuffInfo.range = sizeof(RenderMatrices);

		dsetWritesFrame[0] = {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = descriptorSets[DSET_IDX_FRAME],
			.dstBinding = 0,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.pBufferInfo = &viewProjBuffInfo
		};

		// all matrices
		const VkDeviceSize offsetMatrices = offsetUniform(sizeof(RenderMatrices));
		VkDescriptorBufferInfo matrixBuffInfo = {};
		matrixBuffInfo.buffer = matrixBuffer.buffer;
		matrixBuffInfo.offset = offsetMatrices;
		matrixBuffInfo.range = sizeof(mat4) * MAX_OBJECTS;

		dsetWritesFrame[1] = {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = descriptorSets[DSET_IDX_FRAME],
			.dstBinding = 1,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.pBufferInfo = &matrixBuffInfo
		};

		// update shader material data
		const BufferInfo &materialBuffer = buffers.get(materialBufferId);
		VkDescriptorBufferInfo materialsBuffInfo = {};
		materialsBuffInfo.buffer = materialBuffer.buffer;
		materialsBuffInfo.offset = 0;
		materialsBuffInfo.range = sizeof(ShaderMaterial) * maxMaterials;

		dsetWritesFrame[2] = {
			.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
			.dstSet = descriptorSets[DSET_IDX_FRAME],
			.dstBinding = 2,
			.dstArrayElement = 0,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.pBufferInfo = &materialsBuffInfo
		};

		vkUpdateDescriptorSets(device, dsetWritesFrame.size(), dsetWritesFrame.data(), 0, nullptr);

		// update lights descriptor
		const BufferInfo &lightsBuffer = buffers.get(lightsBufferId);
		VkDescriptorBufferInfo lightsBuffInfo = {};
		lightsBuffInfo.buffer = lightsBuffer.buffer;
		lightsBuffInfo.offset = 0;
		lightsBuffInfo.range = sizeof(LightSource) * maxLights;

		// update input attachments
		std::array<VkDescriptorImageInfo, 3> descriptorImages{};
		descriptorImages[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		descriptorImages[0].imageView = gBuffers[imageIndex].positions.imageView;
		descriptorImages[0].sampler = VK_NULL_HANDLE;
		descriptorImages[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		descriptorImages[1].imageView = gBuffers[imageIndex].albedo.imageView;
		descriptorImages[1].sampler = VK_NULL_HANDLE;
		descriptorImages[2].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		descriptorImages[2].imageView = gBuffers[imageIndex].normals.imageView;
		descriptorImages[2].sampler = VK_NULL_HANDLE;

		const VkDescriptorSet deferredDescSet = deferredDescriptors.getSet(frameInfo.currentFrame);
		std::array<VkWriteDescriptorSet, 4> descriptorWrites{};
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstSet = deferredDescSet;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pImageInfo = &descriptorImages[0];
		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstSet = deferredDescSet;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &descriptorImages[1];
		descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[2].dstBinding = 2;
		descriptorWrites[2].dstSet = deferredDescSet;
		descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		descriptorWrites[2].descriptorCount = 1;
		descriptorWrites[2].pImageInfo = &descriptorImages[2];

		descriptorWrites[3].dstBinding = 3;
		descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[3].dstSet = deferredDescSet;
		descriptorWrites[3].dstArrayElement = 0;
		descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[3].descriptorCount = 1;
		descriptorWrites[3].pBufferInfo = &lightsBuffInfo;

		vkUpdateDescriptorSets(device, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
	}
	else if (nextImageResult == VK_ERROR_OUT_OF_DATE_KHR || resizeOccured)
	{
		if (resizeOccured)
		{
			logger.log("Resize detected");
		}
		logger.log("Invalidated swapchain, recreating...");
		// handle out of date images
		resizeOccured = false;
		recreateSwapchain();

		// once swapchain is recreated, try again
		shouldRender = false;
		logger.log("Swapchain recreated, command buffers reinitialized");
	}
	else
	{
		logger.error("Image acquire error code: {}", static_cast<unsigned int>(nextImageResult));
		throw std::runtime_error("Error during image aquire");
	}

	return shouldRender;
}

VkDeviceSize VulkanRenderer::offsetUniform(VkDeviceSize offset)
{
	VkDeviceSize minimumOffset = deviceProperties.limits.minUniformBufferOffsetAlignment;
	return (offset < minimumOffset) ? minimumOffset : offset;
}

void VulkanRenderer::render(AssetSet &assetSet, const FrameInfo &frameInfo, const std::vector<MaterialGroup> &materialGroups, RenderStage stage)
{
	const short currentFrame = frameInfo.currentFrame;

	VkCommandBuffer commandBuffer = geometryCommandBuffers[currentFrame];
	VkPipelineLayout pipelineLayout = gBufferPipeline.vulkanLayout();
	VkPipeline pipeline = gBufferPipeline.vulkanPipeline();
	VkSampler sampler = textureSampler.vkSampler();
	if (stage == RenderStage::ALPHA_BLENDING)
	{
		commandBuffer = alphaCommandBuffers[currentFrame];
		pipeline = alphaBufferPipeline.vulkanPipeline();
	}
	else if (stage == RenderStage::POST_DEFERRED_LIGHTING)
	{
		commandBuffer = deferredCommandBuffers[currentFrame];
	}
	else if (stage == RenderStage::POST_DEPTH_WRITE)
	{
		commandBuffer = skyboxCommandBuffers[currentFrame];
		pipeline = skyboxPipeline.vulkanPipeline();
		sampler = cubemapSampler.vkSampler();
	}
	else if (stage == RenderStage::UI)
	{
		commandBuffer = uiCommandBuffers[currentFrame];
		pipelineLayout = uiPipeline.vulkanLayout();
		pipeline = uiPipeline.vulkanPipeline();
		sampler = glyphAtlasSampler.vkSampler();
	}
	else if (stage == RenderStage::TEXT)
	{
		commandBuffer = uiCommandBuffers[currentFrame];
		pipelineLayout = glyphPipeline.vulkanLayout();
		pipeline = glyphPipeline.vulkanPipeline();
		sampler = glyphAtlasSampler.vkSampler();
	}
	else if (stage == RenderStage::DEBUG)
	{
		commandBuffer = debugCommandBuffers[currentFrame];
		pipelineLayout = debugLinePipeline.vulkanLayout();
		pipeline = debugLinePipeline.vulkanPipeline();
	}
	
	// setup descriptor sets
	static std::array<VkDescriptorSet, 2> descriptorSets{};
	descriptorSets[DSET_IDX_FRAME] = renderDescriptors.getSet(currentFrame);

	for (unsigned int i = 0; i < static_cast<unsigned int>(materialGroups.size()); ++i)
	{
		const MaterialGroup &group = materialGroups[i];

		// TODO: Avoid looping over all material groups
		// only loop over the ones that are actually being used
		if (group.materialId != Asset::NO_ASSET)
		{
			const Material &material = assetSet.materials.get(group.materialId);
			const uint32_t descriptorIndex = (currentFrame * maxMaterials) + group.materialId; // TODO: Careful using ID as index
			descriptorSets[DSET_IDX_MATERIAL] = materialDescriptors.getSet(descriptorIndex);
			const int bindDescCount = descriptorSets.size();

			if (material.baseTexture != Asset::NO_ASSET) // TODO: Add pipeline and descriptor layout without base texture sampler
			{
				VkDescriptorImageInfo imageInfo = {};
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.imageView = textures.get(material.baseTexture).imageView;
				imageInfo.sampler = sampler;

				// update the per-material descriptor
				std::array<VkWriteDescriptorSet, 1> dsetObjWrites;
				dsetObjWrites[0] = {
					.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
					.dstSet = descriptorSets[DSET_IDX_MATERIAL],
					.dstBinding = 0,
					.dstArrayElement = 0,
					.descriptorCount = 1,
					.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
					.pImageInfo = &imageInfo,
				};

				vkUpdateDescriptorSets(device, dsetObjWrites.size(), dsetObjWrites.data(), 0, nullptr);
			}
			// bind the appropriate pipeline for this material
			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0,
									bindDescCount, descriptorSets.data(), 0, nullptr);
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
							  material.baseTexture == Asset::NO_ASSET && stage != RenderStage::DEBUG ? gBufferNoTexPipeline.vulkanPipeline() : pipeline);

			for (const MaterialGroup::PrimitiveInstance &instance : group.primitives)
			{
				RenderConstants constants;
				constants.materialId = group.materialId;
				constants.matrixId = instance.matrixId;
				constants.offset = vec4(instance.drawOffset, 0);
				vkCmdPushConstants(commandBuffer, pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
								   0, sizeof(RenderConstants), &constants);

				// draw the vertex data
				const Primitive &primitive = assetSet.primitives.get(instance.primitiveId);
				const PrimitiveBuffers &primitiveBuffers = primitives.get(primitive.bufferId);
				const BufferInfo &vertexBuffer = buffers.get(primitiveBuffers.getVertexBufferId());
				const BufferInfo &indexBuffer = buffers.get(primitiveBuffers.getIndexBufferId());

				// TODO: Group bind/draw commands by buffer ID as well
				const std::array<VkBuffer, 1> buffers = {vertexBuffer.buffer};
				const std::array<VkDeviceSize, 1> offsets = {instance.vertexBufferStart};
				uint32_t indexCount = instance.indexCount == 0 ? primitive.indexCount() : instance.indexCount;

				vkCmdBindVertexBuffers(commandBuffer, 0, buffers.size(), buffers.data(), offsets.data());
				vkCmdBindIndexBuffer(commandBuffer, indexBuffer.buffer, instance.indexBufferStart, VK_INDEX_TYPE_UINT32);

				// apply scissors if needed
				if (instance.shouldClip)
				{
					assert(stage == RenderStage::UI);

					std::array<VkRect2D, 1> scissors{};
					scissors[0].offset.x = static_cast<unsigned int>(instance.clip.position.x);
					scissors[0].offset.y = static_cast<unsigned int>(instance.clip.position.y);
					scissors[0].extent.width = static_cast<unsigned int>(instance.clip.size.width);
					scissors[0].extent.height = static_cast<unsigned int>(instance.clip.size.height);
					vkCmdSetScissor(commandBuffer, 0, scissors.size(), scissors.data());
				}

				vkCmdDrawIndexed(commandBuffer, indexCount, 1, instance.indexOffset, instance.vertexOffset, 0);
			}
		}
	}
}

void VulkanRenderer::displayFrame(const FrameInfo &frameInfo, AssetSet &assetSet)
{
    if (nextImageResult == VK_SUCCESS || nextImageResult == VK_SUBOPTIMAL_KHR)
	{
		const VkCommandBuffer commandBuffer = commandBuffers[frameInfo.currentFrame];

		// deferred pass setup
		VkCommandBuffer deferredCommandBuffer = deferredCommandBuffers[frameInfo.currentFrame];
		const VkDescriptorSet deferredDescSet = deferredDescriptors.getSet(frameInfo.currentFrame);
		vkCmdBindDescriptorSets(deferredCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, deferredPipelineLayout, 0, 1, &deferredDescSet, 0, nullptr);
		vkCmdBindPipeline(deferredCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, deferredPipeline.vulkanPipeline());

		// deferred shading subpass
		GBufferPushConstants consts;
		consts.cameraPosition = cameraPosition;
		vkCmdPushConstants(deferredCommandBuffer, deferredPipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(GBufferPushConstants), &consts);
		vkCmdDraw(deferredCommandBuffer, 3, 1, 0, 0);

		if (vkEndCommandBuffer(geometryCommandBuffers[frameInfo.currentFrame]) != VK_SUCCESS)
		{
			throw std::runtime_error("Error ending the command buffer");
		}
		if (vkEndCommandBuffer(alphaCommandBuffers[frameInfo.currentFrame]) != VK_SUCCESS)
		{
			throw std::runtime_error("Error ending the command buffer");
		}
		if (vkEndCommandBuffer(deferredCommandBuffers[frameInfo.currentFrame]) != VK_SUCCESS)
		{
			throw std::runtime_error("Error ending the command buffer");
		}
		if (vkEndCommandBuffer(skyboxCommandBuffers[frameInfo.currentFrame]) != VK_SUCCESS)
		{
			throw std::runtime_error("Error ending the command buffer");
		}
		if (vkEndCommandBuffer(uiCommandBuffers[frameInfo.currentFrame]) != VK_SUCCESS)
		{
			throw std::runtime_error("Error ending the command buffer");
		}
		if (vkEndCommandBuffer(debugCommandBuffers[frameInfo.currentFrame]) != VK_SUCCESS)
		{
			throw std::runtime_error("Error ending the command buffer");
		}

		// sequence primary command buffer
		vkCmdExecuteCommands(commandBuffer, 1, &geometryCommandBuffers[frameInfo.currentFrame]);
		vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
		vkCmdExecuteCommands(commandBuffer, 1, &alphaCommandBuffers[frameInfo.currentFrame]);
		vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
		vkCmdExecuteCommands(commandBuffer, 1, &deferredCommandBuffers[frameInfo.currentFrame]);
		vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
		vkCmdExecuteCommands(commandBuffer, 1, &skyboxCommandBuffers[frameInfo.currentFrame]);
		vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
		vkCmdExecuteCommands(commandBuffer, 1, &debugCommandBuffers[frameInfo.currentFrame]);
		vkCmdExecuteCommands(commandBuffer, 1, &uiCommandBuffers[frameInfo.currentFrame]);

		// matrix data updates
		mat4 projection = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 500.0f);
		mat4 orthographic = glm::ortho(0.0f, static_cast<cgfloat>(swapChainExtent.width), static_cast<cgfloat>(swapChainExtent.height), 0.0f);
		RenderMatrices renderMatrices {
			.view = viewMatrix,
			.projection = projection,
			.viewProjection = projection * viewMatrix,
			.orthographic = orthographic
		};

		// update view-matrix data
		const BufferInfo &matrixBuffer = buffers.get(matrixBufferId);
		void *viewMatrixData = nullptr;
		vkMapMemory(device, matrixBuffer.memory, 0, sizeof(RenderMatrices), 0, &viewMatrixData);
		memcpy(viewMatrixData, &renderMatrices, sizeof(RenderMatrices));
		vkUnmapMemory(device, matrixBuffer.memory);

		// update matrix data
		const VkDeviceSize offsetMatrices = offsetUniform(sizeof(RenderMatrices));
		void *matrixData = nullptr;
		const VkDeviceSize size = matrices.byteSize();
		vkMapMemory(device, matrixBuffer.memory, offsetMatrices, size, 0, &matrixData);
		memcpy(matrixData, matrices.data(), size);
		vkUnmapMemory(device, matrixBuffer.memory);

		// update materials buffer
		std::vector<ShaderMaterial> shaderMaterials(assetSet.materials.getSize());
		for (unsigned int i = 0; i < static_cast<unsigned int>(assetSet.materials.getSize()); ++i)
		{
			const Material &material = assetSet.materials.getAssets()[i];
			shaderMaterials[i].baseColorFactor = material.diffuse;
		}
		updateMaterials(shaderMaterials);

		vkCmdEndRenderPass(commandBuffer);

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Error ending the command buffer");
		}

		// semaphore is used to signal the end of a frame, so the next can start
		// semaphore indexes match up with the stages at the respective array index
		std::array<VkSemaphore, 1> waitSemaphores = {imageSemaphores[frameInfo.currentFrame]};
		std::array<VkPipelineStageFlags, 1> waitStages = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = waitSemaphores.size();
		submitInfo.pWaitSemaphores = waitSemaphores.data();
		submitInfo.pWaitDstStageMask = waitStages.data();
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		// render semaphores are signalled when queue commands are finished, and waited on for presentation
		std::array<VkSemaphore, 1> signalSemaphores = {renderSemaphores[frameInfo.currentFrame]};
		submitInfo.signalSemaphoreCount = signalSemaphores.size();
		submitInfo.pSignalSemaphores = signalSemaphores.data();

		// fence used to let CPU know we're done executing the command buffer
		if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, frameFences[frameInfo.currentFrame]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to submit draw command buffer to the graphics queue");
		}

		// submit render result back to swap chain for presentation
		std::array<VkSwapchainKHR, 1> presentSwapChains = {swapChain};
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = signalSemaphores.size();
		presentInfo.pWaitSemaphores = signalSemaphores.data();
		presentInfo.swapchainCount = presentSwapChains.size();
		presentInfo.pSwapchains = presentSwapChains.data();
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;

		vkQueuePresentKHR(presentationQueue, &presentInfo);
	}
	else
	{
		logger.error("Unable to display frame, image wasn't acquired");
	}
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
													VkDebugUtilsMessageTypeFlagsEXT,
													const VkDebugUtilsMessengerCallbackDataEXT *callbackData,
													void *userData)
{
	Logger *logger = static_cast<Logger *>(userData);
	if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
	{
		logger->error("{}\n", callbackData->pMessage);
	}
	else
	{
		logger->log(callbackData->pMessage);
	}

	return VK_FALSE;
}
