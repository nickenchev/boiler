#ifndef IMGUIVULKAN_H
#define IMGUIVULKAN_H

#include "gui/guihandler.h"
#include "core/logger.h"

union SDL_Event;

namespace Boiler
{

class ImGuiVulkan : public Boiler::GUIHandler
{
	Logger logger;
public:
	ImGuiVulkan() : logger("ImGui") { }

	void initialize(const Renderer &renderer) override;
	void cleanup() const override;
	void processEvent(const SDL_Event &event) const override;
	void preRender() const override;
	void render() const override;
};

}

#endif /* IMGUIVULKAN_H */
