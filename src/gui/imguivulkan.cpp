#include <vector>
#include "SDL2/SDL_events.h"
#include "gui/imguivulkan.h"
#include "display/imaging/imagedata.h"
#include "core/components/transformcomponent.h"

using namespace Boiler;

void ImGuiVulkan::initialize(Renderer &renderer)
{
}

void ImGuiVulkan::cleanup() const
{
}

void ImGuiVulkan::processEvent(const SDL_Event &event) const
{
}

void ImGuiVulkan::preRender(Renderer &renderer) const
{
}

void ImGuiVulkan::render(Renderer &renderer, const FrameInfo &frameInfo)
{
}

void ImGuiVulkan::postRender(Renderer &renderer) const
{
}
