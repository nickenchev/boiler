#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <vector>

#include "core/math.h"
#include "core/logger.h"
#include "core/common.h"
#include "animation/common.h"
#include "animation/clip.h"
#include "animation/animation.h"
#include "animation/animationsampler.h"
#include "animation/components/animationcomponent.h"
#include "core/entitycomponentsystem.h"
#include "core/components/transformcomponent.h"

namespace Boiler
{

class Animator
{
	Logger logger;

	std::vector<AnimationSampler> samplers;
	std::vector<Animation> animations;

	EntityComponentSystem &ecs;

public:
    Animator(EntityComponentSystem &ecs) : logger("Animator"), ecs(ecs)
	{
	}

	SamplerId addSampler(const AnimationSampler &&sampler)
	{
		samplers.push_back(sampler);
		SamplerId newIdx = samplers.size() - 1;
		return newIdx;
	}

    AnimationId addAnimation(const Animation &&animation)
	{
		animations.push_back(animation);
        AnimationId newIndex = animations.size() - 1;
        return newIndex;
	}

	void animate(Time globalTime, Time deltaTime, AnimationComponent &animationComponent)
	{
		for (const Clip &clip : animationComponent.getClips())
		{
			if (clip.getGlobalStart() <= globalTime)
			{
				const Animation &animation = animations[clip.getAnimationId()];
				const auto &targets = animationComponent.getTargets();
				for (const auto &channel : animation.getChannels())
				{
					TransformComponent &transform = ecs.getComponentStore().retrieve<TransformComponent>(targets[channel.getTarget()]);
					const AnimationSampler &sampler = samplers[channel.getSamplerId()]; // TODO: Change this so not using index

					if (channel.getPath() == Path::TRANSLATION)
					{
						transform.setPosition(sampler.sample<vec3>(globalTime));
					}
					else if (channel.getPath() == Path::ROTATION)
					{
						const auto value = sampler.sample<vec4>(globalTime);
						transform.setOrientation(quat(value.w, value.x, value.y, value.z));
					}
					else if (channel.getPath() == Path::SCALE)
					{
						transform.setScale(sampler.sample<vec3>(globalTime));
					}
				}
			}
		}
	}
};

};

#endif /* ANIMATOR_H */
