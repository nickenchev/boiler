#ifndef CLIP_H
#define CLIP_H

#include "core/common.h"
#include "animation/common.h"

namespace Boiler
{

class Clip
{
	Time globalStart;
	Time localTime;
	AnimationId animationId;
	bool continuous;

public:
	Clip(Time globalStart, AnimationId animationId, bool continuous = false)
	{
		this->globalStart = globalStart;
		this->animationId = animationId;
		this->continuous = continuous;
	}

	Time getGlobalStart() const { return globalStart; }
	void advance(float deltaTime)
	{
		localTime += deltaTime;
	}

	AnimationId getAnimationId() const { return animationId; }
};

};
#endif /* CLIP_H */
