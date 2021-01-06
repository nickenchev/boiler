#ifndef ANIMATIONSYSTEM_H
#define ANIMATIONSYSTEM_H

#include "core/system.h"

namespace Boiler
{

class Animator;

class AnimationSystem : public System
{
	Animator &animator;
	double totalTime;

public:
	AnimationSystem(Animator &animator) : System("Animation System"), animator(animator)
	{
		totalTime = 0;
	}

	void update(ComponentStore &store, const double delta) override;
};

};

#endif /* ANIMATIONSYSTEM_H */
