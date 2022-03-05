#ifndef IMGUIVULKAN_H
#define IMGUIVULKAN_H

#include "gui/guihandler.h"
#include "core/logger.h"
#include "display/vulkanrenderer.h"

union SDL_Event;

namespace Boiler
{

class ImGuiVulkan : public Boiler::GUIHandler
{
	Logger logger;
	Vulkan::VulkanRenderer &renderer;

public:
	ImGuiVulkan(Vulkan::VulkanRenderer &renderer) : logger("ImGui"), renderer(renderer) { }

	void initialize() override;
	void cleanup() const override;
	void processEvent(const SDL_Event &event) const override;
	void preRender() const override;
	void render() const override;
};

}

#endif /* IMGUIVULKAN_H */
