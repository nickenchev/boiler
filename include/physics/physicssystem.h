#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include "core/system.h"

namespace Boiler
{
class MatrixCache;

class PhysicsSystem : public System
{
	MatrixCache &matrixCache;
public:
    PhysicsSystem(MatrixCache &matrixCache);
    void update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, EntityComponentSystem &ecs) override;
};

}

#endif /* PHYSICSSYSTEM_H */
