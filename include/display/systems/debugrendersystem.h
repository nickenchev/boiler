#ifndef DEBUGRENDERSYSTEM_H
#define DEBUGRENDERSYSTEM_H

#include <core/system.h>
#include <assets/assetset.h>

namespace Boiler
{

class Renderer;
class MatrixCache;

class DebugRenderSystem : public System
{
	AssetSet assetSet;
	std::vector<AssetId> primitiveIds;
	AssetId materialId;
	MatrixCache &matrixCache;
	bool enabled;

public:
	DebugRenderSystem(Renderer &renderer, MatrixCache &matrixCache);

    void update(Renderer &renderer, AssetSet &asseSet, const FrameInfo &frameInfo, EntityComponentSystem &ecs) override;
	bool getEnabled() const { return enabled; }
	void setEnabled(bool enabled)
	{
		this->enabled = enabled;
	}
};

};

#endif /* DEBUGRENDERSYSTEM_H */
