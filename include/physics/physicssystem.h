#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include "core/system.h"

namespace Boiler
{

class PhysicsSystem : public System
{
public:
    PhysicsSystem();
    void update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, EntityComponentSystem &ecs) override;
};

}

#endif /* PHYSICSSYSTEM_H */
