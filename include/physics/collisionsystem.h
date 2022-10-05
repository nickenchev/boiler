#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "core/system.h"

namespace Boiler
{

class CollisionSystem : public System
{
public:
	CollisionSystem();

    void update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, EntityComponentSystem &ecs) override;
};

};

#endif // !COLLISIONSYSTEM_H
