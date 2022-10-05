#ifndef LIGHTINGSYSTEM_H
#define LIGHTINGSYSTEM_H

#include "core/system.h"

namespace Boiler
{

class Renderer;

class LightingSystem: public Boiler::System
{
public:
	LightingSystem();

    void update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, EntityComponentSystem &ecs) override;
};

}

#endif
