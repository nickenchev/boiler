#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <vector>
#include "core/logger.h"
#include "animation/animation.h"
#include "core/entitycomponentsystem.h"

namespace Boiler
{

class Animator
{
	Logger logger;
	float totalTime;
	std::vector<Animation> animations;
	EntityComponentSystem &ecs;

public:
    Animator(EntityComponentSystem &ecs) : logger("Animator"), ecs(ecs)
	{
		totalTime = 0;
	}

	void addAnimation(const Animation &&animation)
	{
		animations.push_back(animation);

	}

	void animate(double delta)
	{
		totalTime += delta;
		logger.log("Total time: {}", totalTime);

		for (const auto &animation : animations)
		{
			for (const auto &channel : animation.getChannels())
			{
				const AnimationSampler &sampler = animation.getSampler(channel.getSamplerIndex());
				sampler.sample<float>(totalTime);
			}
		}
	}
};

};

#endif /* ANIMATOR_H */
