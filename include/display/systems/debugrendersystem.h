#ifndef DEBUGRENDERSYSTEM_H
#define DEBUGRENDERSYSTEM_H

#include "core/system.h"

namespace Boiler
{

class Renderer;
class MatrixCache;

class DebugRenderSystem : public System
{
	std::vector<AssetId> primitiveIds;
	AssetId materialId;
	MatrixCache &matrixCache;

public:
	DebugRenderSystem(Renderer &renderer, MatrixCache &matrixCache);

    void update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, EntityComponentSystem &ecs) override;
};

};

#endif /* DEBUGRENDERSYSTEM_H */
