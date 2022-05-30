#ifndef ANIMATIONSYSTEM_H
#define ANIMATIONSYSTEM_H

#include "core/common.h"
#include "core/system.h"

namespace Boiler
{

class Animator;

class AnimationSystem : public System
{
	Animator &animator;

public:
	AnimationSystem(Animator &animator);

	void update(AssetSet &assetSet, const FrameInfo &frameInfo, ComponentStore &store) override;
};

};

#endif /* ANIMATIONSYSTEM_H */
