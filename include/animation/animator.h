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

	const Animation &getAnimation(AnimationId animationId) const
	{
		return animations[animationId];
	}

	Clip createClip(Time globalStart, AnimationId animationId, bool continuous = false) const
	{
		//const Animation &animation = getAnimation(animationId);
		return Clip(globalStart, animationId, continuous);
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
		for (Clip &clip : animationComponent.getClips())
		{
			if (clip.getGlobalStart() <= globalTime)
			{
				const Animation &animation = getAnimation(clip.getAnimationId());
				const auto &targets = animationComponent.getTargets();

				for (const auto &channel : animation.getChannels())
				{
					TransformComponent &transform = ecs.getComponentStore().retrieve<TransformComponent>(targets[channel.getTarget()]);
					const AnimationSampler &sampler = samplers[channel.getSamplerId()]; // TODO: Change this so not using index

					if (channel.getPath() == Path::TRANSLATION)
					{
						transform.translation = sampler.sample<vec3>(clip.getLocalTime());
					}
					else if (channel.getPath() == Path::ROTATION)
					{
						const auto value = sampler.sample<vec4>(clip.getLocalTime());
						transform.orientation = quat(value.w, value.x, value.y, value.z);
					}
					else if (channel.getPath() == Path::SCALE)
					{
						transform.scale = sampler.sample<vec3>(clip.getLocalTime());
					}
				}
				clip.advance(deltaTime);
			}
		}
	}
};

};

#endif /* ANIMATOR_H */
