#ifndef GUIHANDLER_H
#define GUIHANDLER_H

#include <video/renderer.h>
#include <SDL2/SDL.h>

class GUIHandler
{
public:
    GUIHandler()
	{
	}
    virtual ~GUIHandler()
	{
	}

	virtual void initialize(const Renderer &renderer) const = 0;
	virtual void cleanup() const = 0;
	virtual void processEvents(const SDL_Event &event) const = 0;
	virtual void preRender() const = 0;
	virtual void render() const = 0;
};


#endif /* GUIHANDLER_H */
