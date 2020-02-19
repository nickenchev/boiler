#include "video/vulkanrenderer.h"

#include <array>
#include <chrono>
#include <SDL.h>
#include <SDL_vulkan.h>
#include <SDL_syswm.h>
//#include <X11/Xlib-xcb.h>
#include <cstring>
#include <memory>
#include <stdexcept>
#include <sys/types.h>
#include <utility>
#include <vector>
#include <SDL.h>
#include <SDL_vulkan.h>
#include <SDL_syswm.h>
#include <fmt/format.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "SDL_atomic.h"
#include "core/logger.h"
#include "core/math.h"
#include "video/spvshaderprogram.h"
#include "video/vulkanmodel.h"
#include "video/modelviewprojection.h"
#include "vulkan/vulkan_core.h"
#include "video/vktexture.h"

using namespace Boiler;
constexpr bool enableValidationLayers = true;
constexpr bool enableDebugMessages = true;
constexpr int maxFramesInFlight = 2;
constexpr int maxAnistrophy = 16;
constexpr int maxObjects = 100;

int descriptorId = 0;

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
													VkDebugUtilsMessageTypeFlagsEXT messageType,
													const VkDebugUtilsMessengerCallbackDataEXT *callbackData,
													void *userData);

auto getFunctionPointer(VkInstance instance, std::string funcName)
{
	auto func = vkGetInstanceProcAddr(instance, funcName.c_str());
	if (!func)
	{
		throw std::runtime_error("Couldn't find Vulkan function: " + funcName);
	}
	return func;
}

VulkanRenderer::VulkanRenderer() : Renderer("Vulkan Renderer")
{
	currentFrame = 0;
	resizeOccured = false;
}

void VulkanRenderer::prepareShutdown()
{
	// wait for all queues to empty prior to cleaning up
	// validation layers can cause memory leaks without this
	vkDeviceWaitIdle(device);
}

VulkanRenderer::~VulkanRenderer()
{
	cleanupSwapchain();

	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

	// delete the shader module
	if (program)
	{
		program.reset();
	}
	
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
		auto destroyFunc = (PFN_vkDestroyDebugUtilsMessengerEXT)getFunctionPointer(instance, funcName.c_str());
		destroyFunc(instance, debugMessenger, nullptr);
	}

	vkDestroySampler(device, textureSampler, nullptr);
	logger.log("Destroyed sampler");

	// cleanup Vulkan device and instance
	vkDestroyDevice(device, nullptr);
	logger.log("Device destroyed");
	vkDestroySurfaceKHR(instance, surface, nullptr);
	logger.log("Surface destroyed");
	vkDestroyInstance(instance, nullptr);
	logger.log("Instance destroyed");

	if (win)
	{
		SDL_DestroyWindow(win);
	}
}

void VulkanRenderer::initialize(const Size &size)
{
	Renderer::initialize(size);
	
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == 0)
    {
		SDL_WindowFlags winFlags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
		win = SDL_CreateWindow("Boiler", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
							   getScreenSize().width, getScreenSize().height, winFlags);

        if (win)
        {
			// query supported extensions
			uint32_t extensionCount = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

			std::vector<VkExtensionProperties> instProps(extensionCount);
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, instProps.data());

			// query required extensions required by SDL Vulkan
			unsigned int sdlVkExtCount = 0;
			SDL_Vulkan_GetInstanceExtensions(win, &sdlVkExtCount, nullptr);
			std::vector<const char *> sdlVkExts(sdlVkExtCount);
			if (!SDL_Vulkan_GetInstanceExtensions(win, &sdlVkExtCount, sdlVkExts.data()))
			{
				throw std::runtime_error("Unable to find required SDL-Vulkan extensions");
			}

			// copy required extensions into our final requested list
			std::vector<const char *> requestedExtensions;
			for (const char *ext : sdlVkExts)
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
						logger.log(std::string(instProps[i].extensionName) + " will be enabled");
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
			logger.log(std::to_string(extensionCount) + " extensions supported total");

			// create instance
			VkApplicationInfo appInfo = {};
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appInfo.pApplicationName = "Boiler Vulkan Renderer";
			appInfo.pEngineName = "Boiler";
			appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			appInfo.apiVersion = VK_API_VERSION_1_0;

			VkInstanceCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			createInfo.pApplicationInfo = &appInfo;
			createInfo.enabledExtensionCount = requestedExtensions.size();
			createInfo.ppEnabledExtensionNames = requestedExtensions.data();
			createInfo.enabledLayerCount = 0;

			// validation layers
			const std::vector<const char *> validationLayers = { "VK_LAYER_KHRONOS_validation" };
			uint32_t layerCount = 0;
			vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
			std::vector<VkLayerProperties> layers(layerCount);
			vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

			if constexpr (enableValidationLayers)
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

				logger.log(std::to_string(validationLayers.size()) + " layer(s) will be enabled");
				createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
				createInfo.ppEnabledLayerNames = validationLayers.data();
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
				auto createFunc = (PFN_vkCreateDebugUtilsMessengerEXT)getFunctionPointer(instance, funcName.c_str());
				if (createFunc(instance, &debugCreateInfo, nullptr, &debugMessenger) != VK_SUCCESS)
				{
					logger.log("Error setting up debug messenger");
				}
			}

			// create surface
			//VkXcbSurfaceCreateInfoKHR surfaceInfo = {};
			//surfaceInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;

			// get platform specific window handle/data
			/*
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
			if (!SDL_Vulkan_CreateSurface(win, instance, &surface))
			{
				throw std::runtime_error("Unable to create Vulkan surface");
			}
			else
			{
				logger.log("Surface created");
			}

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

				if constexpr(enableValidationLayers)
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
					vkGetDeviceQueue(device, queueFamilyIndices.graphics.value(), 0, &graphicsQueue);
				if (queueFamilyIndices.presentation.has_value())
					vkGetDeviceQueue(device, queueFamilyIndices.presentation.value(), 0, &presentationQueue);
				if (queueFamilyIndices.transfer.has_value())
				{
					vkGetDeviceQueue(device, queueFamilyIndices.transfer.value(), 0, &transferQueue);
				}
				else
				{
					logger.log("Transfer queue will be the same as the graphics queue");
					transferQueue = graphicsQueue;
				}

				// load vertex and fragment SPIR-V shaders
				program = std::make_unique<SPVShaderProgram>(device, "shaders/", "vert.spv", "frag.spv");

				createSwapChain();
				createRenderPass();
				createDescriptorSetLayout();
				createGraphicsPipeline();
				createDepthResources();
				createFramebuffers();
				createCommandPools();
				createMvpBuffers();
				createDescriptorPool();
				createDescriptorSets();
				createCommandBuffers();
				createSynchronization();
				createTextureSampler();
			}
        }
		else
		{
			throw std::runtime_error("Error creating window");
		}
    }
	else
    {
		throw std::runtime_error("Error Initializing SDL: " + std::string(SDL_GetError()));
    }
}

void VulkanRenderer::createSwapChain()
{
	// swap chain and presentation details
	VkSurfaceCapabilitiesKHR surfaceCapabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &surfaceCapabilities);

	// surface formats
	std::vector<VkSurfaceFormatKHR> formats;
	u_int32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
	if (formatCount > 0)
	{
		formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, formats.data());
	}

	// presentation modes
	std::vector<VkPresentModeKHR> presentModes;
	u_int32_t presentModeCount = 0;
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
		if (format.format == VK_FORMAT_B8G8R8_UNORM &&
			format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			selectedFormat = format;
			logger.log("Found preferred surface image format");
		}
	}

	// select the presentation mode
	VkPresentModeKHR selectedPresentMode = VK_PRESENT_MODE_FIFO_KHR;
	for (const auto &presentMode : presentModes)
	{
		if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			selectedPresentMode = presentMode;
			logger.log("Found preferred presentation mode");
		}
	}

	// swap chain extent
	VkExtent2D surfaceExtent;
	surfaceExtent.width = std::max(surfaceCapabilities.minImageExtent.width,
									std::min(surfaceCapabilities.maxImageExtent.width, static_cast<Uint32>(getScreenSize().width)));
	surfaceExtent.height = std::max(surfaceCapabilities.minImageExtent.height,
									std::min(surfaceCapabilities.maxImageExtent.height, static_cast<Uint32>(getScreenSize().height)));

	u_int32_t imageCount = surfaceCapabilities.minImageCount + 1; // extra image to avoid waiting on the driver
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
	std::array<u_int32_t, 2> queueIndices = { queueFamilyIndices.graphics.value(), queueFamilyIndices.presentation.value() };
	if (graphicsQueue != presentationQueue)
	{
		swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapChainCreateInfo.queueFamilyIndexCount = queueIndices.size();
		swapChainCreateInfo.pQueueFamilyIndices = queueIndices.data();
	}
	else
	{
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
	u_int32_t swapImagesCount = 0;
	vkGetSwapchainImagesKHR(device, swapChain, &swapImagesCount, nullptr);
	swapChainImages.resize(swapImagesCount);
	vkGetSwapchainImagesKHR(device, swapChain, &swapImagesCount, swapChainImages.data());
	logger.log("Got swapchain images");

	swapChainFormat = selectedFormat.format;
	swapChainExtent = surfaceExtent;

	// create the image views
	swapChainImageViews.resize(swapChainImages.size());
	for (int i = 0; i < swapChainImages.size(); ++i)
	{
		swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainFormat, VK_IMAGE_ASPECT_COLOR_BIT);
	}
}

void VulkanRenderer::createRenderPass()
{
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = swapChainFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentDescription depthAttachment = {};
	depthAttachment.format = findDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colourAttachRef = {};
	colourAttachRef.attachment = 0;
	colourAttachRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachRef = {};
	depthAttachRef.attachment = 1;
	depthAttachRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colourAttachRef;
	subpass.pDepthStencilAttachment = &depthAttachRef;

	// subpass dependencies
	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0; // index
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	// create the render pass
	std::array<VkAttachmentDescription, 2> attachments = {
		colorAttachment, depthAttachment
	};
	VkRenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = attachments.size();
	renderPassCreateInfo.pAttachments = attachments.data();
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;
	renderPassCreateInfo.dependencyCount = 1;
	renderPassCreateInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &renderPass) != VK_SUCCESS)
	{
		throw std::runtime_error("Error creating render pass.");
	}
}

void VulkanRenderer::createGraphicsPipeline()
{
	// Vertex input stage
	VkVertexInputBindingDescription inputBind = {};
	inputBind.binding = 0;
	inputBind.stride = sizeof(Vertex);
	inputBind.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	std::array<VkVertexInputAttributeDescription, 3> attrDescs = {};
	VkVertexInputAttributeDescription attrDescription;
	// vertex position
	attrDescs[0].binding = 0;
	attrDescs[0].location = 0;
	attrDescs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attrDescs[0].offset = offsetof(Vertex, position);

	// vertex colour
	attrDescs[1].binding = 0;
	attrDescs[1].location = 1;
	attrDescs[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	attrDescs[1].offset = offsetof(Vertex, colour);
	// texture coords
	attrDescs[2].binding = 0;
	attrDescs[2].location = 2;
	attrDescs[2].format = VK_FORMAT_R32G32_SFLOAT;
	attrDescs[2].offset = offsetof(Vertex, textureCoordinate);

	VkPipelineVertexInputStateCreateInfo vertInputCreateInfo = {};
	vertInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertInputCreateInfo.vertexBindingDescriptionCount = 1;
	vertInputCreateInfo.pVertexBindingDescriptions = &inputBind;
	vertInputCreateInfo.vertexAttributeDescriptionCount = attrDescs.size();
	vertInputCreateInfo.pVertexAttributeDescriptions = attrDescs.data();

	// Input assembly stage
	VkPipelineInputAssemblyStateCreateInfo assemblyCreateInfo = {};
	assemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	assemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	assemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

	// Viewport and scissor setup
	VkViewport viewport = {};
	viewport.x = 0;
	viewport.y = 0;
	viewport.width = static_cast<float>(swapChainExtent.width);
	viewport.height = static_cast<float>(swapChainExtent.height);
	viewport.minDepth = 0;
	viewport.maxDepth = 1;

	VkRect2D scissor = {};
	scissor.offset = {0, 0};
	scissor.extent = swapChainExtent;

	VkPipelineViewportStateCreateInfo viewportCreateInfo = {};
	viewportCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportCreateInfo.viewportCount = 1;
	viewportCreateInfo.pViewports = &viewport;
	viewportCreateInfo.scissorCount = 1;
	viewportCreateInfo.pScissors = &scissor;

	// Rasterizer configuration
	VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo = {};
	rasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizerCreateInfo.depthClampEnable = VK_FALSE;
	rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizerCreateInfo.lineWidth = 1.0f;
	rasterizerCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizerCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizerCreateInfo.depthBiasEnable = VK_FALSE;
	rasterizerCreateInfo.depthBiasConstantFactor = 0.0f;
	rasterizerCreateInfo.depthBiasClamp = 0.0f;
	rasterizerCreateInfo.depthBiasSlopeFactor = 0.0f;

	VkPipelineMultisampleStateCreateInfo multiSampCreateInfo = {};
	multiSampCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multiSampCreateInfo.sampleShadingEnable = VK_FALSE;
	multiSampCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multiSampCreateInfo.minSampleShading = 1.0f;
	multiSampCreateInfo.pSampleMask = nullptr;
	multiSampCreateInfo.alphaToCoverageEnable = VK_FALSE;
	multiSampCreateInfo.alphaToOneEnable = VK_FALSE;

	// Colour blending configuration
	VkPipelineColorBlendAttachmentState colorBlendAttachState = {};
	colorBlendAttachState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachState.blendEnable = VK_TRUE;
	colorBlendAttachState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
	colorBlendAttachState.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachState.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo = {};
	colorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendCreateInfo.logicOpEnable = VK_FALSE;
	colorBlendCreateInfo.logicOp = VK_LOGIC_OP_COPY;
	colorBlendCreateInfo.attachmentCount = 1;
	colorBlendCreateInfo.pAttachments = &colorBlendAttachState;
	colorBlendCreateInfo.blendConstants[0] = 0.0f;
	colorBlendCreateInfo.blendConstants[1] = 0.0f;
	colorBlendCreateInfo.blendConstants[2] = 0.0f;
	colorBlendCreateInfo.blendConstants[3] = 0.0f;

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = 1;
	pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Error creating pipeline layout");
	}

	// depth/stencil buffer setup
	VkPipelineDepthStencilStateCreateInfo depthStencilInfo = {};
	depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilInfo.depthTestEnable = VK_TRUE;
	depthStencilInfo.depthWriteEnable = VK_TRUE;
	depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
	depthStencilInfo.stencilTestEnable = VK_FALSE;

	std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages =
		{program->getVertStageInfo(), program->getFragStageInfo()};

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = shaderStages.size();
	pipelineInfo.pStages = shaderStages.data();
	pipelineInfo.pVertexInputState = &vertInputCreateInfo;
	pipelineInfo.pInputAssemblyState = &assemblyCreateInfo;
	pipelineInfo.pViewportState = &viewportCreateInfo;
	pipelineInfo.pRasterizationState = &rasterizerCreateInfo;
	pipelineInfo.pMultisampleState = &multiSampCreateInfo;
	pipelineInfo.pDepthStencilState = &depthStencilInfo;
	pipelineInfo.pColorBlendState = &colorBlendCreateInfo;
	pipelineInfo.pDynamicState = nullptr;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0; // index of subpass

	// not deriving a pipeline
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.basePipelineIndex = -1;

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
	{
		throw std::runtime_error("Error creating graphics pipeline");
	}
	logger.log("Created graphics pipeline");
}

void VulkanRenderer::createFramebuffers()
{
	framebuffers.resize(swapChainImageViews.size());

	for (size_t i = 0; i < swapChainImages.size(); ++i)
	{
		const VkImageView &imageView = swapChainImageViews[i];

		std::array<VkImageView, 2> attachments = {
			swapChainImageViews[i], depthImageView
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

void VulkanRenderer::createDescriptorSetLayout()
{
	std::array<VkDescriptorSetLayoutBinding, 2> bindings = {};

	// UBO binding
	bindings[0].binding = 0;
	bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	bindings[0].descriptorCount = 1;
	bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	bindings[0].pImmutableSamplers = nullptr;

	// sampler binding
	bindings[1].binding = 1;
	bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	bindings[1].descriptorCount = 1;
	bindings[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	bindings[1].pImmutableSamplers = nullptr;
	
	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = bindings.size();
	layoutInfo.pBindings = bindings.data();

	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Error creating descriptor set layout");
	}

	logger.log("Created {} descriptor set layouts", bindings.size());
}

void VulkanRenderer::createDescriptorPool()
{
	std::array<VkDescriptorPoolSize, 2> poolSizes = {};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = maxObjects;

	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = maxObjects;

	VkDescriptorPoolCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	createInfo.poolSizeCount = poolSizes.size();
	createInfo.pPoolSizes = poolSizes.data();
	createInfo.maxSets = maxObjects;

	if (vkCreateDescriptorPool(device, &createInfo, nullptr, &descriptorPool) != VK_SUCCESS)
	{
		throw std::runtime_error("Unable to create descriptor pool");
	}

	logger.log("Created descriptor pool");
}

void VulkanRenderer::createDescriptorSets()
{
	// need to copy layout into array of layouts since create call expects array
	std::vector<VkDescriptorSetLayout> layouts(maxObjects, descriptorSetLayout);

	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = maxObjects;
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(maxObjects);
	if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate descriptor sets");
	}

	logger.log("Allocated {} descriptor sets", descriptorSets.size());
}

void VulkanRenderer::createCommandPools()
{
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
	auto imagePair = createImage(Size(swapChainExtent.width, swapChainExtent.height), depthFormat,
								 VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
								 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	depthImage = imagePair.first;
	depthImageMemory = imagePair.second;
	depthImageView = createImageView(imagePair.first, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
}

void VulkanRenderer::createMvpBuffers()
{
	VkDeviceSize bufferSize = sizeof(ModelViewProjection);
	mvpBuffers.resize(swapChainImages.size());
	mvpBuffersMemory.resize(swapChainImages.size());

	for (int i = 0; i < swapChainImages.size(); ++i)
	{
		auto buffPair = createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		mvpBuffers[i] = buffPair.first;
		mvpBuffersMemory[i] = buffPair.second;
	}
}

void VulkanRenderer::recreateSwapchain()
{
	vkDeviceWaitIdle(device);

	cleanupSwapchain();

	createSwapChain();
	createRenderPass();
	createGraphicsPipeline();
	createDepthResources();
	createFramebuffers();
	createMvpBuffers();
	createDescriptorPool();
	createDescriptorSets();
	createCommandBuffers();
}

uint32_t VulkanRenderer::findMemoryType(uint32_t memoryTypeBits, VkMemoryPropertyFlags propertFlags) const
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

	// TODO: Change MVP buffer cleanup
	for (const auto &buffer : mvpBuffers)
	{
		vkDestroyBuffer(device, buffer, nullptr);
	}
	for (const auto &memory : mvpBuffersMemory)
	{
		vkFreeMemory(device, memory, nullptr);
	}
	logger.log("Cleaned up mvp buffers");

	// command buffers
	vkFreeCommandBuffers(device, commandPool, commandBuffers.size(), commandBuffers.data());
	logger.log("Destroyed command buffers");

	for (const auto &imageView : swapChainImageViews)
	{
		vkDestroyImageView(device, imageView, nullptr);
	}
	logger.log("Destroyed image views");

	vkDestroyDescriptorPool(device, descriptorPool, nullptr);
	logger.log("Destroyed descriptor pool");

	vkDestroySwapchainKHR(device, swapChain, nullptr);
	logger.log("Swapchain destroyed");

	// clean up graphics pipeline
	vkDestroyPipeline(device, graphicsPipeline, nullptr);
	logger.log("Pipeline destroyed");
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	logger.log("Pipeline layout destroyed");
	vkDestroyRenderPass(device, renderPass, nullptr);
	logger.log("Render pass destroyed");
}

void VulkanRenderer::resize(const Boiler::Size &size)
{
	Renderer::resize(size);
	resizeOccured = true;
}

std::string VulkanRenderer::getVersion() const
{
	return "Boiler Vulkan 1.0";
}

std::pair<VkImage, VkDeviceMemory> VulkanRenderer::createImage(const Size &imageSize, VkFormat format, VkImageTiling tiling,
															   VkImageUsageFlags usage, VkMemoryPropertyFlags memProperties) const
{
	VkImage textureImage;
	VkDeviceMemory textureMemory;

	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = imageSize.width;
	imageInfo.extent.height = imageSize.height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.flags = 0;

	if (vkCreateImage(device, &imageInfo, nullptr, &textureImage) != VK_SUCCESS)
	{
		throw std::runtime_error("Error creating image");
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, textureImage, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, memProperties);

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

std::shared_ptr<const Texture> VulkanRenderer::createTexture(const std::string &filePath, const Size &textureSize,
															 const void *pixelData, u_int8_t bytesPerPixel) const
{
	const VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
	//const VkFormat format = VK_FORMAT_R8G8B8A8_SRGB;

	if (bytesPerPixel < 4)
	{
		throw std::runtime_error("Texture image must contain alpha channel");
	}

	VkDeviceSize bytesSize = textureSize.width * textureSize.height * bytesPerPixel;
	auto bufferPair = createBuffer(bytesSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
								   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	logger.log("Staging {} bytes of data", bytesSize);
	void *data = nullptr;
	vkMapMemory(device, bufferPair.second, 0, static_cast<size_t>(bytesSize), 0, &data);
	memcpy(data, pixelData, static_cast<size_t>(bytesSize));
	vkUnmapMemory(device, bufferPair.second);

	auto imagePair = createImage(textureSize, format, VK_IMAGE_TILING_OPTIMAL,
								 VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
								 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);


	// transition the image to transfer layout, copy the buffer pixel data to the image
	transitionImageLayout(imagePair.first, format, VK_IMAGE_LAYOUT_UNDEFINED,
						  VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(bufferPair.first, imagePair.first, textureSize);
	
	// cleanup buffer and memory
	vkDestroyBuffer(device, bufferPair.first, nullptr);
	vkFreeMemory(device, bufferPair.second, nullptr);

	// transition the image to a layout optimal for shader sampling
	transitionImageLayout(imagePair.first, format, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						  VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	VkImageView imageView = createImageView(imagePair.first, format, VK_IMAGE_ASPECT_COLOR_BIT);
	logger.log("Loaded texture data for {}", filePath);

	return std::make_shared<VkTexture>(filePath, device, imagePair.first, imagePair.second, imageView);
}

void VulkanRenderer::setActiveTexture(std::shared_ptr<const Texture> texture) const
{
}

std::pair<VkBuffer, VkDeviceMemory> VulkanRenderer::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
																 VkMemoryPropertyFlags memoryProperties) const
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

	return std::make_pair(buffer, bufferMemory);
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
	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
	{
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
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
	region.imageExtent = {static_cast<u_int32_t>(imageSize.width), static_cast<u_int32_t>(imageSize.height), 1};

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

std::pair<VkBuffer, VkDeviceMemory> VulkanRenderer::createGPUBuffer(void *data, long size, VkBufferUsageFlags usageFlags) const
{
	VkDeviceSize bufferSize = size;
	// create a staging buffer (host), map memory and copy
	auto stageBufferPair = createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
										VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	void *mappedMem = nullptr;
	vkMapMemory(device, stageBufferPair.second, 0, bufferSize, 0, &mappedMem);
	memcpy(mappedMem, data, bufferSize);
	vkUnmapMemory(device, stageBufferPair.second);

	// create a device-local buffer
	auto bufferPair = createBuffer(bufferSize, usageFlags | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
								   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	copyBuffer(stageBufferPair.first, bufferPair.first, bufferSize);

	vkDestroyBuffer(device, stageBufferPair.first, nullptr);
	vkFreeMemory(device, stageBufferPair.second, nullptr);

	return bufferPair;
}

std::shared_ptr<const Model> VulkanRenderer::loadModel(const VertexData &data) const
{
	auto vertexPair = createGPUBuffer((void *)data.vertexBegin(), data.vertexSize(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
	auto indexPair = createGPUBuffer((void *)data.indexBegin(), data.indexSize(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
	logger.log("Created model buffers");

	return std::make_shared<VulkanModel>(device, vertexPair.first, vertexPair.second, indexPair.first, indexPair.second, data, descriptorId++);
}

void VulkanRenderer::beginRender()
{
	nextImageResult = vkAcquireNextImageKHR(device, swapChain, UINT32_MAX, imageSemaphores[currentFrame],
											VK_NULL_HANDLE, &imageIndex);

	if (nextImageResult == VK_SUCCESS)
	{
		// ensure current command buffer has finished executing before attempting to reuse it
		vkWaitForFences(device, 1, &frameFences[currentFrame], VK_TRUE, UINT32_MAX);
		vkResetFences(device, 1, &frameFences[currentFrame]);

		// reset the command buffer
		vkResetCommandBuffer(commandBuffers[imageIndex], 0);

		// submit data to command buffer
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Could not begin command buffer");
		}

		// begin the render pass
		VkRenderPassBeginInfo renderBeginInfo = {};
		renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderBeginInfo.renderPass = renderPass;
		renderBeginInfo.framebuffer = framebuffers[imageIndex];
		renderBeginInfo.renderArea.offset = {0, 0};
		renderBeginInfo.renderArea.extent = swapChainExtent;

		// clear colour
		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = {getClearColor().r, getClearColor().g, getClearColor().b, 1.0f};
		clearValues[1].depthStencil = {1.0f, 0};
		renderBeginInfo.clearValueCount = clearValues.size();
		renderBeginInfo.pClearValues = clearValues.data();

		// perform the render pass
		vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
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

void VulkanRenderer::render(const glm::mat4 modelMatrix, const std::shared_ptr<const Model> model,
							const std::shared_ptr<const Texture> sourceTexture, const TextureInfo *textureInfo,
							const glm::vec4 &colour)
{
	// setup uniforms
	glm::vec3 camPos{0, 0, 2.0f};
	glm::vec3 direction{0, 0, -1.0f};

	ModelViewProjection mvp = {};
	mvp.model = modelMatrix;
	mvp.view = glm::lookAt(camPos, camPos + direction, glm::vec3(0.0f, 1.0f, 0.0f));
	mvp.projection = glm::perspective(glm::radians(45.0f), swapChainExtent.width / (float) swapChainExtent.height, 0.1f, 10.0f);

	// map uniform buffer and copy
	void *data = nullptr;
	vkMapMemory(device, mvpBuffersMemory[imageIndex], 0, sizeof(mvp), 0, &data);
	memcpy(data, &mvp, sizeof(mvp));
	vkUnmapMemory(device, mvpBuffersMemory[imageIndex]);

	auto texture = static_cast<const VkTexture *>(sourceTexture.get());

	// setup descriptor sets
	VkDescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = mvpBuffers[imageIndex];
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(ModelViewProjection);

	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageInfo.imageView = texture->getImageView();
	imageInfo.sampler = textureSampler;

	const VulkanModel *vkmodel = static_cast<const VulkanModel *>(model.get());

	std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};
	// MVP uniform
	descriptorWrites[0].dstBinding = 0;
	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	//descriptorWrites[0].dstSet = descriptorSets[imageIndex];
	descriptorWrites[0].dstSet = descriptorSets[vkmodel->getDescriptorId()];
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo = &bufferInfo;

	// texture
	descriptorWrites[1].dstBinding = 1;
	descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[1].dstSet = descriptorSets[vkmodel->getDescriptorId()];
	descriptorWrites[1].dstArrayElement = 0;
	descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites[1].descriptorCount = 1;
	descriptorWrites[1].pImageInfo = &imageInfo;

	vkUpdateDescriptorSets(device, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
	vkCmdBindDescriptorSets(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[vkmodel->getDescriptorId()], 0, nullptr);

	// draw the vertex data
	const std::array<VkBuffer, 1> buffers = {vkmodel->getVertexBuffer()};
	const std::array<VkDeviceSize, buffers.size()> offsets = {0};

	vkCmdBindVertexBuffers(commandBuffers[imageIndex], 0, buffers.size(), buffers.data(), offsets.data());
	vkCmdBindIndexBuffer(commandBuffers[imageIndex], vkmodel->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

	//vkCmdDraw(commandBuffers[imageIndex], model->getNumVertices(), buffers.size(), 0, 0);
	vkCmdDrawIndexed(commandBuffers[imageIndex], static_cast<uint32_t>(vkmodel->getNumIndices()), 1, 0, 0, 0);
}

void VulkanRenderer::endRender()
{
	if (nextImageResult == VK_SUCCESS)
	{
		vkCmdEndRenderPass(commandBuffers[imageIndex]);

		if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS)
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
		submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

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
	currentFrame = (currentFrame + 1) & maxFramesInFlight;
}

void VulkanRenderer::showMessageBox(const std::string &title, const std::string &message)
{
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
													VkDebugUtilsMessageTypeFlagsEXT messageType,
													const VkDebugUtilsMessengerCallbackDataEXT *callbackData,
													void *userData)
{
	Logger *logger = static_cast<Logger *>(userData);
	if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
	{
		logger->error(callbackData->pMessage);
	}
	else
	{
		logger->log(callbackData->pMessage);
	}

	return VK_FALSE;
}
