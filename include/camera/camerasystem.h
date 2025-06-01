#ifndef CAMERASYSTEM_H
#define CAMERASYSTEM_H

#include "core/system.h"

namespace Boiler
{

class Engine;
class Renderer;

class CameraSystem : public Boiler::System
{
	Engine &engine;
	float prevXFactor, prevYFactor;

public:
	CameraSystem(Engine &engine);

    void update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, EntityComponentSystem &ecs) override;
};

}

#endif /* CAMERASYSTEM_H */
