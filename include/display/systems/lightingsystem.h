#ifndef LIGHTINGSYSTEM_H
#define LIGHTINGSYSTEM_H

#include "core/system.h"

namespace Boiler
{

class Renderer;

class LightingSystem: public Boiler::System
{
	Renderer &renderer;

public:
	LightingSystem(Renderer &renderer) : System("Lighting System"), renderer(renderer)
	{
		logger.log("Setting up lighting system.");
	}

	void update(const FrameInfo &frameInfo, ComponentStore &store) override;
};

}

#endif
