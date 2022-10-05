#ifndef ANIMATIONSYSTEM_H
#define ANIMATIONSYSTEM_H

#include "core/system.h"

namespace Boiler
{

class Animator;

class AnimationSystem : public System
{
	Animator &animator;

public:
	AnimationSystem(Animator &animator);

    void update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, EntityComponentSystem &ecs) override;
};

};

#endif /* ANIMATIONSYSTEM_H */
