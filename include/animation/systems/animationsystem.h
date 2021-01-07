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

	void update(ComponentStore &store, const Time deltaTime, const Time globalTime) override;
};

};

#endif /* ANIMATIONSYSTEM_H */
