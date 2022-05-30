#ifndef GUISYSTEM_H
#define GUISYSTEM_H

#include <memory>
#include "core/common.h"
#include <core/system.h>
#include "gui/guihandler.h"

namespace Boiler
{

class Renderer;

class GUISystem : public System
{
	std::unique_ptr<GUIHandler> guiHandler;

public:
	GUISystem(std::unique_ptr<GUIHandler> guiHandler) : System("GUI System")
	{
		assert(guiHandler != nullptr);

		logger.log("Setting up GUI handler.");
		this->guiHandler = std::move(guiHandler);
		this->guiHandler->initialize();
	}

	~GUISystem()
	{
		logger.log("Cleaning up resources.");
		guiHandler->cleanup();
	}

	void update(AssetSet &assetSet, const FrameInfo &frameInfo, ComponentStore &store) override;
	void processEvent(const SDL_Event &event) const
	{
		guiHandler->processEvent(event);
	}
};

}

#endif /* GUISYSTEM_H */
