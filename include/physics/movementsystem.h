#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "core/system.h"

namespace Boiler
{

class MovementSystem : public System
{
public:
    MovementSystem();

    void update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, EntityComponentSystem &ecs) override;
};

}

#endif /* MOVEMENTSYSTEM_H */
