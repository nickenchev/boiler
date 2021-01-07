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
	Time totalTime;

public:
	AnimationSystem(Animator &animator) : System("Animation System"), animator(animator)
	{
		totalTime = 0;
	}

	void update(ComponentStore &store, const Time delta) override;
};

};

#endif /* ANIMATIONSYSTEM_H */
