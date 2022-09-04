#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "core/common.h"
#include "core/system.h"

namespace Boiler
{

class CollisionSystem : public System
{
public:
	CollisionSystem();

	void update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, ComponentStore &store) override;
};

};

#endif // !COLLISIONSYSTEM_H
