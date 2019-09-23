#include "video/vulkanrenderer.h"

#include <SDL.h>
#include <SDL_vulkan.h>
#include <SDL_syswm.h>
#include <X11/Xlib-xcb.h>
#include <cstring>
#include <vector>

#include "core/math.h"
#include "video/spvshaderprogram.h"

using namespace Boiler;
constexpr bool enableValidationLayers = true;
constexpr bool enableDebugMessages = true;

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
													VkDebugUtilsMessageTypeFlagsEXT messageType,
													const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
													void* userData);

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
}

VulkanRenderer::~VulkanRenderer()
{
}

void VulkanRenderer::initialize(const Size &size)
{
	Renderer::initialize(size);
	
	bool success = false;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) == 0)
    {
		SDL_WindowFlags winFlags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
		win = SDL_CreateWindow("Boiler", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
							   getScreenSize().width, getScreenSize().height, winFlags);

		success = true;
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
						logger.error("Layer: " + std::string(layerName) + " not found");
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
				throw std::runtime_error("No compatible GPUs found");
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

					if (devProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
						devFeats.geometryShader)
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
					std::vector<VkQueueFamilyProperties> queueFamProps(queueFamilyCount);
					vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamProps.data());

					int i = 0;
					for (const auto &queueFamProp : queueFamProps)
					{
						if (queueFamProp.queueCount)
						{
							if (queueFamProp.queueFlags & VK_QUEUE_GRAPHICS_BIT)
							{
								queueFamilyIndices.graphics = i;
							}
							VkBool32 presentationSupport = false;
							vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentationSupport);
							if (presentationSupport)
							{
								queueFamilyIndices.presentation = i;
							}

							// done, found everything we need?
							if (queueFamilyIndices.graphics.has_value() &&
								queueFamilyIndices.presentation.has_value())
							{
								break;
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
				const float queuePriority = 1.0f;
				uniqueQueueIndices = { queueFamilyIndices.graphics.value(),
									   queueFamilyIndices.presentation.value() };
				logger.log("Creating " + std::to_string(uniqueQueueIndices.size()) + " queue(s)");
				std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
				for (uint32_t index : uniqueQueueIndices)
				{
					VkDeviceQueueCreateInfo queueCreateInfo = {};
					queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
					queueCreateInfo.queueFamilyIndex = index;
					queueCreateInfo.queueCount = 1;
					queueCreateInfo.pQueuePriorities = &queuePriority;
					queueCreateInfos.push_back(queueCreateInfo);
				}

				// declare device features
				VkPhysicalDeviceFeatures deviceFeatures = {};

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
				vkGetDeviceQueue(device, queueFamilyIndices.graphics.value(), 0, &graphicsQueue);
				vkGetDeviceQueue(device, queueFamilyIndices.presentation.value(), 0, &presentationQueue);

				createSwapChain();
				createRenderPass();
				createGraphicsPipeline();
				createFramebuffers();
				createCommandPool();
				createCommandBuffers();
				createSemaphores();
			}
        }
    }

    if (!success)
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
		const VkImage &image = swapChainImages[i];
		VkImageViewCreateInfo imageViewCreateInfo = {};
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.image = image;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = swapChainFormat;
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device, &imageViewCreateInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Unable to create swapchain image view");
		}
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

	VkAttachmentReference attachmentRef = {};
	attachmentRef.attachment = 0;
	attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &attachmentRef;

	// subpass dependencies
	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0; // index
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	// create the render pass
	VkRenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = 1;
	renderPassCreateInfo.pAttachments = &colorAttachment;
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
	// load vertex and fragment SPIR-V shaders
	program = std::make_unique<SPVShaderProgram>(device, "shaders/", "vert.spv", "frag.spv");
	SPVShaderProgram *spvProgram = static_cast<SPVShaderProgram *>(program.get());

	// Vertex input stage
	VkPipelineVertexInputStateCreateInfo vertInputCreateInfo = {};
	vertInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertInputCreateInfo.vertexBindingDescriptionCount = 0;
	vertInputCreateInfo.pVertexBindingDescriptions = nullptr;
	vertInputCreateInfo.vertexAttributeDescriptionCount = 0;
	vertInputCreateInfo.pVertexAttributeDescriptions = nullptr;

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
	pipelineLayoutCreateInfo.setLayoutCount = 0;
	pipelineLayoutCreateInfo.pSetLayouts = nullptr;
	pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
	pipelineLayoutCreateInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Error creating pipeline layout");
	}

	std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages =
		{spvProgram->getVertStageInfo(), spvProgram->getFragStageInfo()};

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = shaderStages.size();
	pipelineInfo.pStages = shaderStages.data();
	pipelineInfo.pVertexInputState = &vertInputCreateInfo;
	pipelineInfo.pInputAssemblyState = &assemblyCreateInfo;
	pipelineInfo.pViewportState = &viewportCreateInfo;
	pipelineInfo.pRasterizationState = &rasterizerCreateInfo;
	pipelineInfo.pMultisampleState = &multiSampCreateInfo;
	pipelineInfo.pDepthStencilState = nullptr;
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
			
		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = 1;
		framebufferInfo.pAttachments = &imageView;
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

void VulkanRenderer::createCommandPool()
{
	VkCommandPoolCreateInfo commandPoolInfo = {};
	commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolInfo.queueFamilyIndex = queueFamilyIndices.graphics.value();
	commandPoolInfo.flags = 0;

	if (vkCreateCommandPool(device, &commandPoolInfo, nullptr, &commandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("Error creating command pool");
	}
}

void VulkanRenderer::createCommandBuffers()
{
	commandBuffers.resize(framebuffers.size());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = static_cast<uint32_t>(framebuffers.size());

	if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
	{
		throw std::runtime_error("Error allocating command buffers");
	}

	for (size_t i = 0; i < commandBuffers.size(); ++i)
	{
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Could not begin command buffer");
		}

		// begin the render pass
		VkRenderPassBeginInfo renderBeginInfo = {};
		renderBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderBeginInfo.renderPass = renderPass;
		renderBeginInfo.framebuffer = framebuffers[i];
		renderBeginInfo.renderArea.offset = {0, 0};
		renderBeginInfo.renderArea.extent = swapChainExtent;

		// clear colour
		VkClearValue clearColour = {0, 0, 0, 1.0f};
		renderBeginInfo.clearValueCount = 1;
		renderBeginInfo.pClearValues = &clearColour;

		// perform the render pass
		vkCmdBeginRenderPass(commandBuffers[i], &renderBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
		vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Error ending the render pass");
		}
	}
}

void VulkanRenderer::createSemaphores()
{
	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	if (vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &imageSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &renderSemaphore) != VK_SUCCESS)
	{
		throw std::runtime_error("Unable to create semaphores");
	}
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
													VkDebugUtilsMessageTypeFlagsEXT messageType,
													const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
													void* userData)
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

void VulkanRenderer::shutdown()
{
	Renderer::shutdown();

	// cleanup resources
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
	vkDestroySurfaceKHR(instance, surface, nullptr);
	logger.log("Surface destroyed");

	vkDestroySemaphore(device, imageSemaphore, nullptr);
	vkDestroySemaphore(device, renderSemaphore, nullptr);
	logger.log("Destroed semaphores");

	// clean up graphics pipeline
	vkDestroyPipeline(device, graphicsPipeline, nullptr);
	logger.log("Pipeline destroyed");
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	logger.log("Pipeline layout destroyed");
	vkDestroyRenderPass(device, renderPass, nullptr);
	logger.log("Render pass destroyed");

	// command related
	vkDestroyCommandPool(device, commandPool, nullptr);
	logger.log("Destroyed command pool");

	// clean up debug callback
	if constexpr (enableDebugMessages)
	{
		std::string funcName{"vkDestroyDebugUtilsMessengerEXT"};
		auto destroyFunc = (PFN_vkDestroyDebugUtilsMessengerEXT)getFunctionPointer(instance, funcName.c_str());
		destroyFunc(instance, debugMessenger, nullptr);
	}

	// cleanup Vulkan device and instance
	vkDestroyDevice(device, nullptr);
	logger.log("Device destroyed");
	vkDestroyInstance(instance, nullptr);
	logger.log("Instance destroyed");

	if (win)
	{
		SDL_DestroyWindow(win);
	}
}

std::string VulkanRenderer::getVersion() const
{
	return "Boiler Vulkan 1.0";
}

std::shared_ptr<const Texture> VulkanRenderer::createTexture(const std::string filePath, const Size &textureSize, const void *pixelData) const
{
	return nullptr;
}

void VulkanRenderer::setActiveTexture(std::shared_ptr<const Texture> texture) const
{
}

std::shared_ptr<const Model> VulkanRenderer::loadModel(const VertexData &data) const
{
	return nullptr;
}

void VulkanRenderer::beginRender()
{
}

void VulkanRenderer::endRender()
{
}

void VulkanRenderer::render(const glm::mat4 modelMatrix, const std::shared_ptr<const Model> model,
							const std::shared_ptr<const Texture> sourceTexture, const TextureInfo *textureInfo,
							const glm::vec4 &colour) const
{
	uint32_t imageIndex = 0;
	vkAcquireNextImageKHR(device, swapChain, UINT32_MAX, imageSemaphore, VK_NULL_HANDLE, &imageIndex);

	// semaphore indexes match up with the stages at the respective array index
	std::array<VkSemaphore, 1> waitSemaphores = {imageSemaphore};
	std::array<VkPipelineStageFlags, 1> waitStages = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
	
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = waitSemaphores.size();
	submitInfo.pWaitSemaphores = waitSemaphores.data();
	submitInfo.pWaitDstStageMask = waitStages.data();
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

	std::array<VkSemaphore, 1> signalSemaphores = {renderSemaphore};
	submitInfo.signalSemaphoreCount = signalSemaphores.size();
	submitInfo.pSignalSemaphores = signalSemaphores.data();

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to submit draw command buffer to the graphics queue");
	}
}

void VulkanRenderer::showMessageBox(const std::string &title, const std::string &message)
{
}
