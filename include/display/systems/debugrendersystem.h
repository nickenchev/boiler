#ifndef DEBUGRENDERSYSTEM_H
#define DEBUGRENDERSYSTEM_H

#include "core/system.h"

namespace Boiler
{

class Renderer;

class DebugRenderSystem : public System
{
	std::vector<AssetId> primitiveIds;
	AssetId materialId;

public:
	DebugRenderSystem(Renderer &renderer);

    void update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, EntityComponentSystem &ecs) override;
};

};

#endif /* DEBUGRENDERSYSTEM_H */
