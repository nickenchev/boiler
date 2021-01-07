#ifndef CLIP_H
#define CLIP_H

#include "animation/common.h"

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
};

#endif /* CLIP_H */
