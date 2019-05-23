#ifndef GUISYSTEM_H
#define GUISYSTEM_H

#include <memory>
#include <core/system.h>
#include <video/guihandler.h>

namespace Boiler
{

class Renderer;

class GUISystem : public System
{
	const Renderer &renderer;
	std::unique_ptr<GUIHandler> guiHandler;

public:
	GUISystem(const Renderer &renderer, std::unique_ptr<GUIHandler> guiHandler)
		: System("GUI System"), renderer(renderer)
	{
		assert(guiHandler != nullptr);

		logger.log("Setting up GUI handler.");
		this->guiHandler = std::move(guiHandler);
		this->guiHandler->initialize(this->renderer);
	}

	~GUISystem()
	{
		logger.log("Cleaning up resources.");
		guiHandler->cleanup();
	}

	void update(ComponentStore &store, const double delta) override;
	void processEvent(const SDL_Event &event) const
	{
		guiHandler->processEvent(event);
	}
};

}

#endif /* GUISYSTEM_H */
