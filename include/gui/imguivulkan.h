#ifndef IMGUIVULKAN_H
#define IMGUIVULKAN_H

#include "gui/guihandler.h"
#include "core/logger.h"
#include "assets/assetset.h"

union SDL_Event;

namespace Boiler
{

class ImGuiVulkan : public Boiler::GUIHandler
{
	Logger logger;
	AssetId materialId;
	AssetId primitiveId;

public:
	ImGuiVulkan() : logger("ImGui") { }

	void initialize(Renderer &renderer) override;
	void cleanup() const override;
	void processEvent(const SDL_Event &event) const override;
	void preRender(Renderer &renderer) const override;
	void render(Renderer &renderer, const FrameInfo &frameInfo) override;
	void postRender(Renderer &renderer) const override;
};

}

#endif /* IMGUIVULKAN_H */
