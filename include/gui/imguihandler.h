#ifndef IMGUIHANDLER_H
#define IMGUIHANDLER_H

#include <SDL2/SDL.h>
#include "guihandler.h"

namespace Boiler
{

class Renderer;

class ImGuiHandler : public Boiler::GUIHandler
{
public:
	void initialize(const Renderer &renderer) override
	{
	}
	void cleanup() const override
	{
	}
	void processEvent(const SDL_Event &event) const override {}
	void preRender() const override {}
	void render() const override
	{
	}
};

}

#endif /* IMGUIHANDLER_H */
