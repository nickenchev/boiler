#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "core/common.h"
#include "core/frameinfo.h"
#include "core/system.h"

namespace Boiler
{

class Renderer;
class MatrixCache;

class RenderSystem : public System
{
	MatrixCache &matrixCache;

public:
	RenderSystem(MatrixCache &matrixCache);

    void update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, EntityComponentSystem &ecs) override;
};

}

#endif /* RENDERSYSTEM_H */
