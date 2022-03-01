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
	AnimationSystem(Animator &animator) : System("Animation System"), animator(animator)
	{
	}

	void update(FrameInfo frameInfo, ComponentStore &store) override;
};

};

#endif /* ANIMATIONSYSTEM_H */
