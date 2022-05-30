#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "core/system.h"

namespace Boiler
{

class MovementSystem : public System
{
public:
    MovementSystem();

	void update(AssetSet &assetSet, const FrameInfo &frameInfo, ComponentStore &store) override;
};

}

#endif /* MOVEMENTSYSTEM_H */
