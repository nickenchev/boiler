#include <SDL.h>
#include <SDL_vulkan.h>
#include <cstring>
#include "video/vulkanrenderer.h"

#include "core/math.h"

using namespace Boiler;
constexpr bool enableValidationLayers = true;

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
													VkDebugUtilsMessageTypeFlagsEXT messageType,
													const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
													void* userData);

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
		SDL_WindowFlags winFlags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN);
		win = SDL_CreateWindow("Boiler", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
							   getScreenSize().width, getScreenSize().height, winFlags);

		success = true;
        if (win)
        {
			/*
			unsigned int count;
			SDL_Vulkan_GetInstanceExtensions(win, &count, nullptr);
			std::vector<const char *> sdlExts(count);
			SDL_Vulkan_GetInstanceExtensions(win, &count, sdlExts.data());

			for (auto ext : sdlExts)
			{
				logger.log(ext);
			}
			*/


			// query supported extensions
			uint32_t extensionCount = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

			std::vector<VkExtensionProperties> instProps(extensionCount);
			std::vector<const char *> extensionNames(extensionCount);
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, instProps.data());

			std::vector<const char *> requestedExtensions =
			{
				VK_KHR_SURFACE_EXTENSION_NAME,
				VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
				"VK_KHR_xlib_surface"
			};

			bool extensionsOk = true;
			for (auto extension : requestedExtensions)
			{
				bool supported = false;
				for (int i = 0; i < static_cast<int>(extensionCount); ++i)
				{
					if (std::strcmp(extension, instProps[i].extensionName) == 0)
					{
						supported = true;
						logger.log(std::string(instProps[i].extensionName) + " enabled");
						extensionNames.push_back(instProps[i].extensionName);
					}
				}
				if (!supported)
				{
					extensionsOk = false;
					logger.error("Unsupported extension: " + std::string(extension));
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
			createInfo.ppEnabledExtensionNames = extensionNames.data();
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

			if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
			{
				logger.error("Error creating Vulkan instance");
			}
			else
			{
				logger.log("Instance created");
			}

			// debug messenger setup
			VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
			debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
				VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			debugCreateInfo.pfnUserCallback = debugCallback;
			debugCreateInfo.pUserData = static_cast<void *>(&logger);


			VkDebugUtilsMessengerEXT debugMessenger;
			std::string funcName{"vkCreateDebugUtilsMessengerEXT"};
			auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, funcName.c_str());
			if (func != nullptr)
			{
				if (func(instance, &debugCreateInfo, nullptr, &debugMessenger) != VK_SUCCESS)
				{
					logger.log("Error setting up debug messenger");
				}
			}
			else
			{
				logger.log("Couldn't find Vulkan function: " + funcName);
			}
        }
    }

    if (!success)
    {
        std::cout << "Error Initializing SDL: " << SDL_GetError() << std::endl;
    }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
													VkDebugUtilsMessageTypeFlagsEXT messageType,
													const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
													void* userData)
{
	Logger *logger = static_cast<Logger *>(userData);
	logger->error(callbackData->pMessage);

	return VK_FALSE;
}

void VulkanRenderer::shutdown()
{
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
