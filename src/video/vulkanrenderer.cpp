#include <SDL.h>
#include <cstring>
#include "video/vulkanrenderer.h"

#include "core/math.h"

using namespace Boiler;
constexpr bool enableValidationLayers = true;

VulkanRenderer::VulkanRenderer() : Renderer("Vulkan Renderer")
{
}

void VulkanRenderer::initialize(const Size &size)
{
	bool success = false;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) == 0)
    {
		SDL_WindowFlags winFlags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_SHOWN);
		//win = SDL_CreateWindow("Boiler", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenSize.width, screenSize.height, winFlags);

		setScreenSize(size);
		win = SDL_CreateWindow("Boiler", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
							   getScreenSize().width, getScreenSize().height, winFlags);

        if (win)
        {
			// query supported extensions
			uint32_t extensionCount = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

			std::vector<VkExtensionProperties> instProps(extensionCount);
			std::vector<const char *> extensionNames(extensionCount);
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, instProps.data());

			for (int i = 0; i < static_cast<int>(extensionCount); ++i)
			{
				logger.log(instProps[i].extensionName);
				extensionNames.push_back(instProps[i].extensionName);
			}
			logger.log(std::to_string(extensionCount) + " extensions supported");

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
        }
    }

    if (!success)
    {
        std::cout << "Error Initializing SDL: " << SDL_GetError() << std::endl;
    }
}

void VulkanRenderer::shutdown()
{
	if (win)
	{
		SDL_DestroyWindow(win);
		vkDestroyInstance(instance, nullptr);
		logger.log("Instance destroyed");
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
	SDL_GL_SwapWindow(win);
}

void VulkanRenderer::render(const glm::mat4 modelMatrix, const std::shared_ptr<const Model> model,
							const std::shared_ptr<const Texture> sourceTexture, const TextureInfo *textureInfo,
							const glm::vec4 &colour) const
{
}

void VulkanRenderer::showMessageBox(const std::string &title, const std::string &message)
{
}