#ifndef LIGHTINGSYSTEM_H
#define LIGHTINGSYSTEM_H

#include "core/system.h"

namespace Boiler
{

class Renderer;

class LightingSystem: public Boiler::System
{
	const Renderer &renderer;

public:
	LightingSystem(const Renderer &renderer) : System("Lighting System"), renderer(renderer)
	{
		logger.log("Setting up lighting system.");
	}

	void update(Boiler::ComponentStore &store, const double delta) override;
};

}

#endif
