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

				float time = fmod(totalTime, sampler.getMaxTime()); // loop time
				
				if (channel.getPath() == Path::TRANSLATION)
				{
					pos.frame.position = sampler.sample<vec3>(time);
				}
				else if (channel.getPath() == Path::ROTATION)
				{
					const auto value = sampler.sample<vec4>(time);
					pos.orientation.x = value.x;
					pos.orientation.y = value.y;
					pos.orientation.z = value.z;
					pos.orientation.w = value.w;
				}
			}
		}
		totalTime += delta;
	}
};

};

#endif /* ANIMATOR_H */
