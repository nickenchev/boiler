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
	LightingSystem(Renderer &renderer);

	void update(AssetSet &assetSet, const FrameInfo &frameInfo, ComponentStore &store) override;
};

}

#endif
