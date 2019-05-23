#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "core/system.h"

namespace Boiler
{

class Renderer;

class RenderSystem : public System
{
	const Renderer &renderer;

public:
	RenderSystem(const Renderer &renderer) : System("Render System"), renderer(renderer) { }
	void update(ComponentStore &store, const double delta) override;
};

}

#endif /* RENDERSYSTEM_H */
