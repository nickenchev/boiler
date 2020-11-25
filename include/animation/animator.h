#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <vector>
#include "core/math.h"
#include "core/logger.h"
#include "animation/animation.h"
#include "core/entitycomponentsystem.h"
#include "core/components/positioncomponent.h"

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

	void animate(float delta)
	{
		for (const auto &animation : animations)
		{
			for (const auto &channel : animation.getChannels())
			{
				PositionComponent &pos = ecs.getComponentStore().retrieve<PositionComponent>(channel.getEntity());
				const AnimationSampler &sampler = animation.getSampler(channel.getSamplerIndex());
				if (channel.getPath() == "translation")
				{
					pos.frame.position = sampler.sample<vec3>(totalTime);
				}
				if (channel.getPath() == "rotation")
				{
					const auto value = sampler.sample<vec4>(totalTime);
					pos.orientation = quat(value.w, value.x, value.y, value.z);
				}
			}
		}
		totalTime += delta;
		//if (totalTime > 2) totalTime -= 2;
	}
};

};

#endif /* ANIMATOR_H */
