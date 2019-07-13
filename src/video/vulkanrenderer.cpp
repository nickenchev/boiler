#include <SDL.h>
#include <MoltenVK/mvk_vulkan.h>
#include "video/vulkanrenderer.h"

#include "core/math.h"

using namespace Boiler;

VulkanRenderer::VulkanRenderer() : Renderer("Vulkan Renderer")
{
}

void VulkanRenderer::initialize(const Size &size)
{    bool success = false;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) == 0)
    {
		SDL_WindowFlags winFlags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
		win = SDL_CreateWindow("Boiler", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenSize.width, screenSize.height, winFlags);
		setScreenSize(screenSize);

        if (win)
        {
        }
    }

    if (!success)
    {
        std::cout << "Error Initializing SDL: " << SDL_GetError() << std::endl;
    }
}

void VulkanRenderer::shutdown()
{
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
