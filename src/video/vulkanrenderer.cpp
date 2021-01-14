#include "video/vulkanrenderer.h"
#include "video/renderer.h"

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
#include "video/lightsource.h"
#include "video/imaging/imagedata.h"
#include "video/vertexdata.h"
#include "video/primitive.h"
#include "util/filemanager.h"
#include "video/material.h"
#include "video/materialgroup.h"
#include "video/vulkan/texturerequest.h"

using namespace Boiler;
using namespace Boiler::Vulkan;

constexpr bool enableDebugMessages = true;
constexpr int maxFramesInFlight = 2;
constexpr int maxAnistrophy = 16;
constexpr int maxObjects = 1000;
constexpr int maxLights = 64;
constexpr int maxMaterials = 256;
constexpr int maxSamplers = 1;
//constexpr VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
constexpr VkFormat textureFormat = VK_FORMAT_R8G8B8A8_SRGB;


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

VulkanRenderer::VulkanRenderer(const std::vector<const char *> requiredExtensions, bool enableValidationLayers) : Renderer("Vulkan Renderer")
{
	this->enableValidationLayers = enableValidationLayers;
	cleanedUp = false;
	currentFrame = 0;
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
	createInfo.enabledExtensionCount = requestedExtensions.size();
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
	cleanupSwapchain();

	// command buffers
	vkFreeCommandBuffers(device, commandPool, commandBuffers.size(), commandBuffers.data());
	logger.log("Destroyed command buffers");

	vkDestroyRenderPass(device, renderPass, nullptr);
	logger.log("Render passes destroyed");

	renderDescriptors.cleanup(device);
	materialDescriptors.cleanup(device);
	deferredDescriptors.cleanup(device);
	logger.log("Destroyed descriptors");

	// Cleanup all resources allocated for various assets
	for (const TextureImage &ti : textures.getAssets())
	{
		vkFreeMemory(device, ti.memory, nullptr);
		vkDestroyImage(device, ti.image, nullptr);
		vkDestroyImageView(device, ti.imageView, nullptr);
	}
	for (const PrimitiveBuffers &pb : primitives.getAssets())
	{
		freeBuffer(pb.getVertexBuffer());
		freeBuffer(pb.getIndexBuffer());
	}
	freeBuffer(matrixBuffer);
	freeBuffer(lightsBuffer);
	freeBuffer(materialBuffer);
	logger.log("Cleaned up buffers and memory");

	// delete the shader module
	vkDestroyShaderModule(device, gBufferModules.vertex, nullptr);
	vkDestroyShaderModule(device, gBufferModules.fragment, nullptr);
	vkDestroyShaderModule(device, deferredModules.vertex, nullptr);
	vkDestroyShaderModule(device, deferredModules.fragment, nullptr);
	logger.log("Destroyed shader modules");
	
	// sync objects cleanup
	for (int i = 0; i < maxFramesInFlight; ++i)
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

	if (textureSampler != VK_NULL_HANDLE)
	{
		vkDestroySampler(device, textureSampler, nullptr);
		logger.log("Destroyed sampler");
	}

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
	
	// create surface
	/*
	VkXcbSurfaceCreateInfoKHR surfaceInfo = {};
	surfaceInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;

	// get platform specific window handle/data
	SDL_SysWMinfo windowInfo;
	SDL_VERSION(&windowInfo.version);
	if (!SDL_GetWindowWMInfo(win, &windowInfo))
	{
		throw std::runtime_error("Unable to native window info");
	}

	switch (windowInfo.subsystem)
	{
		case SDL_SYSWM_X11:
		{
			surfaceInfo.connection = XGetXCBConnection(windowInfo.info.x11.display);
			surfaceInfo.window = windowInfo.info.x11.window;
			break;
		}
		default:
		{
			throw std::runtime_error("Platform not supported by Boiler Vulkan");
		}
	}
	*/

	/*
	if (!SDL_Vulkan_CreateSurface(win, instance, &surface))
	{
		throw std::runtime_error("Unable to create Vulkan surface");
	}
	else
	{
		logger.log("Surface created");
	}
	*/

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

		for (VkPhysicalDevice device : physicalDevices)
		{
			VkPhysicalDeviceProperties devProps;
			vkGetPhysicalDeviceProperties(device, &devProps);
			VkPhysicalDeviceFeatures devFeats;
			vkGetPhysicalDeviceFeatures(device, &devFeats);

			if (devProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
				&& devFeats.samplerAnisotropy)
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
		deviceCreateInfo.queueCreateInfoCount = queueCreateInfos.size();
		deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
		deviceCreateInfo.enabledExtensionCount = requestedDeviceExtensions.size();
		deviceCreateInfo.ppEnabledExtensionNames = requestedDeviceExtensions.data();

		if (enableValidationLayers)
		{
			deviceCreateInfo.enabledLayerCount = validationLayers.size();
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

		// shader modules to compose final image
		auto deferredVertData = FileManager::readBinaryFile("shaders/deferred.vert.spv");
		deferredModules.vertex = createShaderModule(deferredVertData);
		auto deferredFragData = FileManager::readBinaryFile("shaders/deferred.frag.spv");
		deferredModules.fragment = createShaderModule(deferredFragData);

		createSwapChain();
		createGBuffers();
		createDepthResources();
		
		renderPass = createRenderPass();
		createFramebuffers();
		createDescriptorSets();
		createGraphicsPipelines();

		createMatrixBuffer();
		createLightBuffer(maxLights);
		createMaterialBuffer();

		commandPool = createCommandPools(queueFamilyIndices, graphicsQueue, transferQueue);
		createCommandBuffers();
		createSynchronization();
		createTextureSampler();
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

void VulkanRenderer::createRenderBuffers()
{
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
		if (format.format == VK_FORMAT_R8G8B8A8_UNORM &&
			format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			selectedFormat = format;
			logger.log("Found preferred surface image format");
		}
	}

	// select the presentation mode
	const VkPresentModeKHR preferredMode = VK_PRESENT_MODE_MAILBOX_KHR;
	VkPresentModeKHR selectedPresentMode = VK_PRESENT_MODE_FIFO_KHR;
	for (const auto &presentMode : presentModes)
	{
		if (presentMode == preferredMode)
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
		swapChainCreateInfo.queueFamilyIndexCount = queueIndices.size();
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

	assert(maxFramesInFlight <= swapImagesCount);

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
	auto createAttachment = [](VkFormat format) -> VkAttachmentDescription
	{
		VkAttachmentDescription attachment = {};
		attachment.format = format;
		attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		return attachment;
	};
	// G Buffer attachments
	VkAttachmentDescription positionAttachment = createAttachment(positionFormat);
	positionAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	VkAttachmentDescription albedoAttachment = createAttachment(albedoFormat);
	albedoAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	VkAttachmentDescription normalAttachment = createAttachment(normalFormat);
	normalAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	// swapchain color attachment
	VkAttachmentDescription colorAttachment = createAttachment(swapChainFormat);
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	// depth/stencil attachment
	VkAttachmentDescription depthAttachment = createAttachment(findDepthFormat());
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

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

	std::array<VkSubpassDescription, 2> subpasses{};
	// gbuffer subpasses
	subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpasses[0].colorAttachmentCount = colorAttachmentRefs.size();
	subpasses[0].pColorAttachments = colorAttachmentRefs.data();
	subpasses[0].pDepthStencilAttachment = &depthAttachRef;
	// deferred subpass
	subpasses[1].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpasses[1].colorAttachmentCount = 1;
	subpasses[1].pColorAttachments = &swapColorAttachRef;
	subpasses[1].inputAttachmentCount = inputColorAttachRefs.size();
	subpasses[1].pInputAttachments = inputColorAttachRefs.data();

	// subpass dependencies
	std::array<VkSubpassDependency, 3> dependencies;
	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	// This dependency transitions the input attachment from color attachment to shader read
	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = 1;
	dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	dependencies[2].srcSubpass = 0;
	dependencies[2].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[2].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[2].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[2].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[2].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[2].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	// create the render pass
	std::array<VkAttachmentDescription, 5> attachments = {
		colorAttachment, depthAttachment, positionAttachment, albedoAttachment, normalAttachment
	};
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
    std::array<VkPushConstantRange, 2> pushConsts{};
    pushConsts[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pushConsts[0].offset = 0;
    pushConsts[0].size = sizeof(RenderConstants);


	std::array<VkDescriptorSetLayout, 2> descriptorLayouts{
		renderDescriptors.getLayout(),
		materialDescriptors.getLayout()
	};

	VkPipelineLayoutCreateInfo gBuffPipeLayoutCreateInfo{};
	gBuffPipeLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	gBuffPipeLayoutCreateInfo.setLayoutCount = descriptorLayouts.size();
	gBuffPipeLayoutCreateInfo.pSetLayouts = descriptorLayouts.data();
	gBuffPipeLayoutCreateInfo.pushConstantRangeCount = 1;
	gBuffPipeLayoutCreateInfo.pPushConstantRanges = &pushConsts[0];

    gBuffersPipelineLayout = createGraphicsPipelineLayout(gBuffPipeLayoutCreateInfo);
	
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
		.offset = offsetof(Vertex, textureCoordinate)
	});
	standardAttrDesc.push_back({
		.location = 3,
		.binding = 0,
		.format = VK_FORMAT_R32G32B32_SFLOAT,
		.offset = offsetof(Vertex, normal)
	});

	// pipeline for g-buffer
	gBufferPipeline = GraphicsPipeline::create(device, renderPass, gBuffersPipelineLayout, swapChainExtent, &standardInputBind,
											   &standardAttrDesc, 3, gBufferModules, 0, VK_CULL_MODE_BACK_BIT, true, true);
	// depth disabled pipeline
	depthlessPipeline = GraphicsPipeline::create(device, renderPass, gBuffersPipelineLayout, swapChainExtent, &standardInputBind,
												 &standardAttrDesc, 3, gBufferModules, 0, VK_CULL_MODE_BACK_BIT, false, true);
	// pipeline for deferred final output
	deferredPipeline = GraphicsPipeline::create(device, renderPass, deferredPipelineLayout, swapChainExtent, nullptr, nullptr,
												1, deferredModules, 1, VK_CULL_MODE_FRONT_BIT, true);
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
			depthImageView,
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
	const int frames = maxFramesInFlight;
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

	std::array<VkDescriptorPoolSize, 3> renderPoolSizes{};
	renderPoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	renderPoolSizes[0].descriptorCount = renderBindings[0].descriptorCount * renderDescriptors.getMaxSets();
	renderPoolSizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	renderPoolSizes[1].descriptorCount = renderBindings[1].descriptorCount * renderDescriptors.getMaxSets();
	renderPoolSizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	renderPoolSizes[2].descriptorCount = renderBindings[2].descriptorCount * renderDescriptors.getMaxSets();

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
	commandBuffers.resize(framebuffers.size());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = static_cast<uint32_t>(framebuffers.size());

	if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Error allocating command buffers");
	}

	logger.log("Allocated {} command buffers", commandBuffers.size());
}

void VulkanRenderer::createSynchronization()
{
	imageSemaphores.resize(maxFramesInFlight);
	renderSemaphores.resize(maxFramesInFlight);
	frameFences.resize(maxFramesInFlight);

	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (int i = 0; i < maxFramesInFlight; ++i)
	{
		if (vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &imageSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &renderSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(device, &fenceCreateInfo, nullptr, &frameFences[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Unable to create synchronization objects");
		}
	}
}

void VulkanRenderer::createTextureSampler()
{
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;
	samplerInfo.maxAnisotropy = maxAnistrophy;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	if (vkCreateSampler(device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
	{
		throw std::runtime_error("Error creating sampler");
	}
}

void VulkanRenderer::createDepthResources()
{
	VkFormat depthFormat = findDepthFormat();
	/*
	auto imagePair = createImage(Size(swapChainExtent.width, swapChainExtent.height), depthFormat,
								 VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
								 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	*/

	TextureRequest request(Size(swapChainExtent.width, swapChainExtent.height), depthFormat);
	request.tiling = VK_IMAGE_TILING_OPTIMAL;
	request.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	request.memProperties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	auto imagePair = createImage(request);

	depthImage = imagePair.first;
	depthImageMemory = imagePair.second;
	depthImageView = createImageView(imagePair.first, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
}

void VulkanRenderer::createMatrixBuffer()
{
	const VkDeviceSize size = maxObjects * sizeof(mat4) + sizeof(ViewProjection);
	matrixBuffer = createBuffer(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
								VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void VulkanRenderer::createLightBuffer(int lightCount)
{
	VkDeviceSize bufferSize = sizeof(LightSource) * lightCount;
	lightsBuffer = createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
								VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void VulkanRenderer::createMaterialBuffer()
{
	VkDeviceSize bufferSize = sizeof(ShaderMaterial) * maxMaterials;
	materialBuffer = createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
								  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
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
	vkDestroyImage(device, depthImage, nullptr);
	vkFreeMemory(device, depthImageMemory, nullptr);
	vkDestroyImageView(device, depthImageView, nullptr);
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
	GraphicsPipeline::destroy(device, depthlessPipeline);
	GraphicsPipeline::destroy(device, deferredPipeline);

	vkDestroyPipelineLayout(device, gBuffersPipelineLayout, nullptr);
	vkDestroyPipelineLayout(device, deferredPipelineLayout, nullptr);
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

VkImageView VulkanRenderer::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) const
{
	VkImageViewCreateInfo imageViewCreateInfo = {};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.image = image;
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.format = format;
	imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	imageViewCreateInfo.subresourceRange.aspectMask = aspectFlags;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	if (vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageView) != VK_SUCCESS)
	{
		throw std::runtime_error("Unable to create swapchain image view");
	}

	return imageView;
}

constexpr size_t cubeMapSize = 6;
Texture VulkanRenderer::loadCubemap(const std::array<ImageData, cubeMapSize> &images)
{
	size_t totalSize = 0;

	for (const ImageData &imageData : images)
	{
		totalSize += imageData.size.width * imageData.size.height * imageData.colorComponents;
	}

	// create a single image with 6 layers
	BufferInfo buffInfo = createBuffer(totalSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
									   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	// stage data for all 6 images
	void *data = nullptr;
	vkMapMemory(device, buffInfo.memory, 0, totalSize, 0, &data);
	size_t offset = 0;
	for (auto &image : images)
	{
		const size_t size = image.size.width * image.size.height * image.colorComponents;
		memcpy(data, image.pixelData, size);
	}
	vkUnmapMemory(device, buffInfo.memory);

	// create a multi-layer texture
	TextureRequest request(images[0].size, textureFormat);
	request.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	request.layers = cubeMapSize;
	auto imagePair = createImage(request);

	// copy staged data to VkImage and set baseArrayLayer for each pixel data
	// create an imageview with type cubemap
	// create a cubemap sampler
}

Texture VulkanRenderer::loadTexture(const ImageData &imageData)
{
	const size_t bytesPerPixel = imageData.colorComponents;
	if (imageData.colorComponents < 4)
	{
		throw std::runtime_error("Texture image must contain alpha channel");
	}

	const VkDeviceSize bytesSize = imageData.size.width * imageData.size.height * bytesPerPixel;
	auto bufferInfo = createBuffer(bytesSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
								   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	logger.log("Staging {} bytes of data", bytesSize);
	void *data = nullptr;
	vkMapMemory(device, bufferInfo.memory, 0, static_cast<size_t>(bytesSize), 0, &data);
	memcpy(data, imageData.pixelData, static_cast<size_t>(bytesSize));
	vkUnmapMemory(device, bufferInfo.memory);

	TextureRequest request(imageData.size, textureFormat);
	request.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	auto imagePair = createImage(request);

	// transition the image to transfer layout, copy the buffer pixel data to the image
	transitionImageLayout(imagePair.first, textureFormat, VK_IMAGE_LAYOUT_UNDEFINED,
						  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(bufferInfo.buffer, imagePair.first, imageData.size);
	
	// cleanup buffer and memory
	freeBuffer(bufferInfo);

	// transition the image to a layout optimal for shader sampling
	transitionImageLayout(imagePair.first, textureFormat, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	VkImageView imageView = createImageView(imagePair.first, textureFormat, VK_IMAGE_ASPECT_COLOR_BIT);

	logger.log("Texture data loaded");

	TextureImage texImage;
	texImage.image = imagePair.first;
	texImage.memory = imagePair.second;
	texImage.imageView = imageView;
	return textures.add(texImage);
}

BufferInfo VulkanRenderer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryProperties) const
{
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
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

	return BufferInfo(buffer, bufferMemory, size);
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

void VulkanRenderer::copyBuffer(VkBuffer &srcBuffer, VkBuffer dstBuffer, VkDeviceSize dataSize) const
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands(transferPool);
	
	VkBufferCopy copyRegion = {};
	copyRegion.size = dataSize;
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

	endSingleTimeCommands(transferQueue, transferPool, commandBuffer);
}

void VulkanRenderer::transitionImageLayout(VkImage image, VkFormat format,
										   VkImageLayout oldLayout, VkImageLayout newLayout) const
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
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

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

void VulkanRenderer::copyBufferToImage(VkBuffer buffer, VkImage image, const Size &imageSize) const
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands(transferPool);

	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

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

BufferInfo VulkanRenderer::createGPUBuffer(void *data, long size, VkBufferUsageFlags usageFlags) const
{
	VkDeviceSize bufferSize = size;
	// create a staging buffer (host), map memory and copy
	auto stageBufferInfo = createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
										VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void *mappedMem = nullptr;
	vkMapMemory(device, stageBufferInfo.memory, 0, bufferSize, 0, &mappedMem);
	memcpy(mappedMem, data, bufferSize);
	vkUnmapMemory(device, stageBufferInfo.memory);

	// create a device-local buffer
	auto bufferInfo = createBuffer(bufferSize, usageFlags | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
								   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	copyBuffer(stageBufferInfo.buffer, bufferInfo.buffer, bufferInfo.size);

	vkDestroyBuffer(device, stageBufferInfo.buffer, nullptr);
	vkFreeMemory(device, stageBufferInfo.memory, nullptr);

	return bufferInfo;
}

Primitive VulkanRenderer::loadPrimitive(const VertexData &data)
{
	// primitive buffers
	auto vertexBuffer = createGPUBuffer((void *)data.vertexBegin(), data.vertexSize(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
	auto indexBuffer = createGPUBuffer((void *)data.indexBegin(), data.indexSize(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
	AssetId primitiveId = primitives.add(PrimitiveBuffers(vertexBuffer, indexBuffer));

	logger.log("Loaded primitive with asset-id: {}", primitiveId);
	/*
	// MVP buffer
	VkDeviceSize bufferSize = sizeof(ModelViewProjection);
	auto mvpPair = createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
								VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	resourceSet.buffers.push_back(mvpPair.first);
	resourceSet.deviceMemory.push_back(mvpPair.second);

	// material buffer
	// TODO: Don't store materials per object, handle differently
	auto materialPair = createBuffer(sizeof(ShaderMaterial), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
								VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	resourceSet.buffers.push_back(materialPair.first);
	resourceSet.deviceMemory.push_back(materialPair.second);

	resourceSets.push_back(resourceSet);
	*/
	return Primitive(primitiveId, data.vertexArray().size(), data.indexArray().size());
}

Material &VulkanRenderer::createMaterial()
{
	Material &material = Renderer::createMaterial();
	return material;
}

void VulkanRenderer::updateLights(const std::vector<LightSource> &lightSources)
{
	// copy light source data
	void *lightData = nullptr;
	VkDeviceSize lightsMemSize = maxLights * sizeof(LightSource);
	vkMapMemory(device, lightsBuffer.memory, 0, lightsMemSize, 0, &lightData);
	memcpy(lightData, lightSources.data(), lightsMemSize);
	vkUnmapMemory(device, lightsBuffer.memory);
}

void VulkanRenderer::updateMatrices(const std::vector<mat4> &matrices) const
{
	assert(matrices.size() < maxObjects);
	const VkDeviceSize size = matrices.size() * sizeof(mat4);

	void *data = nullptr;
	vkMapMemory(device, matrixBuffer.memory, 0, size, 0, &data);
	memcpy(data, matrices.data(), size);
	vkUnmapMemory(device, matrixBuffer.memory);
}
void VulkanRenderer::updateMaterials(const std::vector<ShaderMaterial> &materials) const
{
	assert(materials.size() < maxMaterials);
	const VkDeviceSize size = materials.size() * sizeof(ShaderMaterial);

	void *data = nullptr;
	vkMapMemory(device, materialBuffer.memory, 0, size, 0, &data);
	memcpy(data, materials.data(), size);
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

void VulkanRenderer::beginRender()
{
	Renderer::beginRender();

	nextImageResult = vkAcquireNextImageKHR(device, swapChain, UINT32_MAX, imageSemaphores[currentFrame],
											VK_NULL_HANDLE, &imageIndex);

	if (nextImageResult == VK_SUCCESS)
	{
		// ensure current command buffer has finished executing before attempting to reuse it
		vkWaitForFences(device, 1, &frameFences[currentFrame], VK_TRUE, UINT32_MAX);
		vkResetFences(device, 1, &frameFences[currentFrame]);

		// setup the command buffers
		const VkCommandBuffer commandBuffer = commandBuffers[currentFrame];
		vkResetCommandBuffer(commandBuffer, 0);

		// submit data to command buffer
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
        clearValues[3].color = {{0, 0, 0, 0}};
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

		// perform the render pass
		vkCmdBeginRenderPass(commandBuffer, &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}
	else if (nextImageResult == VK_ERROR_OUT_OF_DATE_KHR || resizeOccured)
	{
		logger.log("Invalidated swapchain, recreating...");
		// handle out of date images
		resizeOccured = false;
		recreateSwapchain();

		// once swapchain is recreated, try again
		beginRender();
		logger.log("Swapchain recreated, command buffers reinitialized");
	}
	else
	{
		throw std::runtime_error("Error during image aquire");
	}
}

void VulkanRenderer::render(const std::vector<mat4> &matrices, const std::vector<MaterialGroup> &materialGroups)
{
	constexpr size_t DSET_IDX_FRAME = 0;
	constexpr size_t DSET_IDX_MATERIAL = 1;

	const VkCommandBuffer commandBuffer = commandBuffers[currentFrame];

	// setup descriptor sets
	static std::array<VkDescriptorSet, 2> descriptorSets{};
	descriptorSets[DSET_IDX_FRAME] = renderDescriptors.getSet(currentFrame);
	std::array<VkWriteDescriptorSet, 3> dsetWritesFrame{};

	// matrix data updates
	ViewProjection viewProjection {
		.view = viewMatrix,
		.projection = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float)swapChainExtent.height, 0.1f, 500.0f)
	};

	// update matrix data
	const VkDeviceSize size = matrices.size() * sizeof(mat4);
	void *data = nullptr;
	vkMapMemory(device, matrixBuffer.memory, 0, sizeof(ViewProjection), 0, &data);
	memcpy(data, &viewProjection, sizeof(ViewProjection));
	vkUnmapMemory(device, matrixBuffer.memory);

	vkMapMemory(device, matrixBuffer.memory, sizeof(ViewProjection), size, 0, &data);
	memcpy(data, matrices.data(), size);
	vkUnmapMemory(device, matrixBuffer.memory);

	// view and projection matrices
	VkDescriptorBufferInfo viewProjBuffInfo = {};
	viewProjBuffInfo.buffer = matrixBuffer.buffer;
	viewProjBuffInfo.offset = 0;
	viewProjBuffInfo.range = sizeof(ViewProjection);

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
	VkDescriptorBufferInfo matrixBuffInfo = {};
	matrixBuffInfo.buffer = matrixBuffer.buffer;
	matrixBuffInfo.offset = sizeof(ViewProjection);
	matrixBuffInfo.range = sizeof(mat4) * maxObjects;

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
	std::vector<ShaderMaterial> shaderMaterials(getMaterials().size());
	for (int i = 0; i < getMaterials().size(); ++i)
	{
		const Material &material = getMaterials()[i];
		shaderMaterials[i].baseColorFactor = material.color;
		if (material.baseTexture.has_value())
		{
			shaderMaterials[i].useBaseTexture = true;
		}
	}
	updateMaterials(shaderMaterials);

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

	for (int i = 0; i < materialGroups.size(); ++i)
	{
		const MaterialGroup &group = materialGroups[i];

		// TODO: Avoid looping over all material groups
		// only loop over the ones that are actually being used
		if (group.materialId != Asset::NO_ASSET)
		{
			const Material &material = getMaterial(group.materialId);

			// bind the appropriate pipeline for this material
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
							gBufferPipeline.vulkanPipeline());

			std::array<VkWriteDescriptorSet, 1> dsetObjWrites;
			const uint32_t descriptorIndex = (currentFrame * maxMaterials) + i;
			descriptorSets[DSET_IDX_MATERIAL] = materialDescriptors.getSet(descriptorIndex);
			const int bindDescCount = descriptorSets.size();

			if (material.baseTexture.has_value())
			{
				VkDescriptorImageInfo imageInfo = {};
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				imageInfo.imageView = textures.getAsset(material.baseTexture.value().getAssetId()).imageView;
				imageInfo.sampler = textureSampler;

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

			vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, gBuffersPipelineLayout, 0, bindDescCount, descriptorSets.data(), 0, nullptr);

			for (const MaterialGroup::PrimitiveInstance &instance : group.primitives)
			{
				RenderConstants constants;
				constants.materialId = group.materialId;
				constants.matrixId = instance.matrixId;
				vkCmdPushConstants(commandBuffer, gBuffersPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
								0, sizeof(RenderConstants), &constants);

				// draw the vertex data
				const PrimitiveBuffers &primitiveBuffers = primitives.getAsset(instance.primitive.getAssetId());
				const std::array<VkBuffer, 1> buffers = {primitiveBuffers.getVertexBuffer().buffer};
				const std::array<VkDeviceSize, buffers.size()> offsets = {0};

				vkCmdBindVertexBuffers(commandBuffer, 0, buffers.size(), buffers.data(), offsets.data());
				vkCmdBindIndexBuffer(commandBuffer, primitiveBuffers.getIndexBuffer().buffer, 0, VK_INDEX_TYPE_UINT32);
				vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(instance.primitive.getIndexCount()), 1, 0, 0, 0);
			}
		}
	}
}

void VulkanRenderer::endRender()
{
	if (nextImageResult == VK_SUCCESS)
	{
		const VkCommandBuffer commandBuffer = commandBuffers[currentFrame];

		// update input attachments
		std::array<VkDescriptorImageInfo, 3> descriptorImages{};
		descriptorImages[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		descriptorImages[0].imageView = gBuffers[currentFrame].positions.imageView;
		descriptorImages[0].sampler = VK_NULL_HANDLE;
		descriptorImages[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		descriptorImages[1].imageView = gBuffers[currentFrame].albedo.imageView;
		descriptorImages[1].sampler = VK_NULL_HANDLE;
		descriptorImages[2].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		descriptorImages[2].imageView = gBuffers[currentFrame].normals.imageView;
		descriptorImages[2].sampler = VK_NULL_HANDLE;

		const VkDescriptorSet descriptorSet = deferredDescriptors.getSet(currentFrame);
		std::array<VkWriteDescriptorSet, 4> descriptorWrites{};
		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstSet = descriptorSet;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pImageInfo = &descriptorImages[0];
		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstSet = descriptorSet;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &descriptorImages[1];
		descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[2].dstBinding = 2;
		descriptorWrites[2].dstSet = descriptorSet;
		descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		descriptorWrites[2].descriptorCount = 1;
		descriptorWrites[2].pImageInfo = &descriptorImages[2];

		// update lights descriptor
		VkDescriptorBufferInfo lightsBuffInfo = {};
		lightsBuffInfo.buffer = lightsBuffer.buffer;
		lightsBuffInfo.offset = 0;
		lightsBuffInfo.range = sizeof(LightSource) * maxLights;

		descriptorWrites[3].dstBinding = 3;
		descriptorWrites[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[3].dstSet = descriptorSet;
		descriptorWrites[3].dstArrayElement = 0;
		descriptorWrites[3].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[3].descriptorCount = 1;
		descriptorWrites[3].pBufferInfo = &lightsBuffInfo;

		vkUpdateDescriptorSets(device, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, deferredPipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
						  deferredPipeline.vulkanPipeline());

		vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);

		GBufferPushConstants consts;
		consts.cameraPosition = cameraPosition;
		vkCmdPushConstants(commandBuffer, deferredPipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(GBufferPushConstants), &consts);

		vkCmdDraw(commandBuffer, 3, 1, 0, 0);
		vkCmdEndRenderPass(commandBuffer);

		if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Error ending the command buffer");
		}

		// semaphore is used to signal the end of a frame, so the next can start
		// semaphore indexes match up with the stages at the respective array index
		std::array<VkSemaphore, 1> waitSemaphores = {imageSemaphores[currentFrame]};
		std::array<VkPipelineStageFlags, 1> waitStages = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = waitSemaphores.size();
		submitInfo.pWaitSemaphores = waitSemaphores.data();
		submitInfo.pWaitDstStageMask = waitStages.data();
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		std::array<VkSemaphore, 1> signalSemaphores = {renderSemaphores[currentFrame]};
		submitInfo.signalSemaphoreCount = signalSemaphores.size();
		submitInfo.pSignalSemaphores = signalSemaphores.data();

		if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, frameFences[currentFrame]) != VK_SUCCESS)
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
	currentFrame = (currentFrame + 1) % maxFramesInFlight;
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
