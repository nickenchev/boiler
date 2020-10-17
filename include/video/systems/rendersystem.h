#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "core/system.h"
#include "core/lightsourceid.h"
#include "video/material.h"
#include "video/lightsource.h"

namespace Boiler
{

class Renderer;

class RenderSystem : public System
{
	Renderer &renderer;

public:
	RenderSystem(Renderer &renderer) : System("Render System"), renderer(renderer)
	{
	}

	void update(ComponentStore &store, const double delta) override;
};

}

#endif /* RENDERSYSTEM_H */
