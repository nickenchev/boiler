#ifndef GUIHANDLER_H
#define GUIHANDLER_H

#include "display/renderer.h"

union SDL_Event;

namespace Boiler
{

class GUIHandler
{
public:
    GUIHandler()
	{
	}
    virtual ~GUIHandler()
	{
	}

	virtual void initialize(Renderer &renderer) = 0;
	virtual void cleanup() const = 0;
	virtual void processEvent(const SDL_Event &event) const = 0;
	virtual void preRender(Renderer &renderer) const = 0;
	virtual void render(Renderer &renderer, const FrameInfo &frameInfo) = 0;
	virtual void postRender(Renderer &renderer) const = 0;
};

}

#endif /* GUIHANDLER_H */
