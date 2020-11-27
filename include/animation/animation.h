#ifndef ANIMATION_H
#define ANIMATION_H

#include <string>
#include <vector>
#include "animationsampler.h"
#include "channel.h"

namespace Boiler
{

using AnimationId = size_t;

class Animation
{
	std::string name;
	std::vector<AnimationSampler> samplers;
	std::vector<Channel> channels;

public:
    Animation(const std::string &name)
	{
		this->name = name;
	}

	const std::string &getName() const { return name; }

	void addSampler(const AnimationSampler &&sampler)
	{
		samplers.push_back(sampler);
	}

	void addChannel(const Channel &channel)
	{
		channels.push_back(channel);
	}

	const AnimationSampler &getSampler(unsigned int samplerIndex) const
	{
		return samplers.at(samplerIndex);
	}

	const std::vector<Channel> &getChannels() const
	{
		return channels;
	}
};

}
#endif /* ANIMATION_H */
