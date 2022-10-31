#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "core/system.h"

namespace Boiler
{

class MatrixCache;

class CollisionSystem : public System
{
	MatrixCache &matrixCache;
public:
	CollisionSystem(MatrixCache &matrixCache);

    void update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, EntityComponentSystem &ecs) override;
};

};

#endif // !COLLISIONSYSTEM_H
