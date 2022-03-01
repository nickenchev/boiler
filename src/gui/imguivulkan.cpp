#include "SDL_events.h"
#include "gui/imguivulkan.h"

#include "../ext/imgui/imgui_impl_vulkan.h"

using namespace Boiler;

void ImGuiVulkan::initialize(const Renderer &renderer)
{
	logger.log("Initializing");
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
}

void ImGuiVulkan::cleanup() const
{
	logger.log("Shutting down");
	ImGui::DestroyContext();
}

void ImGuiVulkan::processEvent(const SDL_Event &event) const
{
}

void ImGuiVulkan::preRender() const
{
}

void ImGuiVulkan::render() const
{
	ImDrawData *drawData = ImGui::GetDrawData();

	// get current renderer frame->command buffer
}
