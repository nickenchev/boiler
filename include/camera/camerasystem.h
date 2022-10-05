#ifndef CAMERASYSTEM_H
#define CAMERASYSTEM_H

#include "core/system.h"

namespace Boiler
{

class Renderer;

class CameraSystem : public Boiler::System
{
	float prevXFactor, prevYFactor;

public:
	CameraSystem();

    void update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, EntityComponentSystem &ecs) override;
};

}

#endif /* CAMERASYSTEM_H */
