#ifndef GUISYSTEM_H
#define GUISYSTEM_H

#include <memory>
#include <core/system.h>
#include <video/guihandler.h>

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
		this->guiHandler = std::move(guiHandler);
		this->guiHandler->initialize(this->renderer);
	}

	void update(ComponentStore &store, const double delta) const override;
};


#endif /* GUISYSTEM_H */
