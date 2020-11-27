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

using AnimationId = size_t;

class AnimationRunner
{
	const Animation &animation;
	float time;

public:
	AnimationRunner(const Animation &animation) : animation(animation)
	{
		time = 0;
	}

	void step(float delta)
	{
		time += delta;
	}
};

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

    AnimationId addAnimation(const Animation &&animation)
	{
		animations.push_back(animation);
        size_t newIndex = animations.size() - 1;
        return newIndex;
	}

	void resetTime() { totalTime = 0; }

	void animate(float delta)
	{
		for (const auto &animation : animations)
		{
			float time = totalTime;
			for (const auto &channel : animation.getChannels())
			{
				PositionComponent &pos = ecs.getComponentStore().retrieve<PositionComponent>(channel.getEntity());
				const AnimationSampler &sampler = animation.getSampler(channel.getSamplerIndex());

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
				else if (channel.getPath() == Path::SCALE)
				{
					pos.scale = sampler.sample<vec3>(time);
				}
			}
		}
		totalTime += delta;
	}
};

};

#endif /* ANIMATOR_H */
