#include "video/vulkanrenderer.h"

#include <SDL.h>
#include <SDL_vulkan.h>
#include <SDL_syswm.h>
#include <X11/Xlib-xcb.h>
#include <cstring>
#include <vector>
#include <set>

#include "core/math.h"

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
	bool success = false;
	setScreenSize(size);

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
			VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
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
				QueueFamilyIndices queueFamilyIndices;
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

				// create command queue, using set to ensure only unique queue indices are used
				const float queuePriority = 1.0f;
				std::set<uint32_t> uniqueQueueIndices = { queueFamilyIndices.graphics.value(),
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
			}
        }
    }

    if (!success)
    {
		throw std::runtime_error("Error Initializing SDL: " + std::string(SDL_GetError()));
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
	if constexpr (enableDebugMessages)
	{
		std::string funcName{"vkDestroyDebugUtilsMessengerEXT"};
		auto destroyFunc = (PFN_vkDestroyDebugUtilsMessengerEXT)getFunctionPointer(instance, funcName.c_str());
		destroyFunc(instance, debugMessenger, nullptr);
	}

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
}

void VulkanRenderer::showMessageBox(const std::string &title, const std::string &message)
{
}
