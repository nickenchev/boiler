#ifndef CAMERASYSTEM_H
#define CAMERASYSTEM_H

#include "core/system.h"

namespace Boiler
{

class Renderer;

class CameraSystem : public Boiler::System
{
	Renderer &renderer;
	float prevXFactor, prevYFactor;

public:
	CameraSystem(Renderer &renderer);

	void update(AssetSet &assetSet, const FrameInfo &frameInfo, ComponentStore &store) override;
};

}

#endif /* CAMERASYSTEM_H */
