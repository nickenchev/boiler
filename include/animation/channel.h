#ifndef CHANNEL_H
#define CHANNEL_H

#include <string>
#include <vector>
#include "core/entity.h"
#include "animationsampler.h"

namespace Boiler
{

using ChannelTarget = unsigned int;

enum class Path
{
	TRANSLATION,
	ROTATION,
	SCALE,
};

class Channel
{
	ChannelTarget target;
	Path path;
	unsigned int samplerIndex;

public:
	Channel(ChannelTarget target, const std::string &path, unsigned int samplerIndex)
	{
		this->target = target;
		this->samplerIndex = samplerIndex;

		if (path == "translation")
		{
			this->path = Path::TRANSLATION;
		}
		else if (path == "rotation")
		{
			this->path = Path::ROTATION;
		}
		else if (path == "scale")
		{
			this->path = Path::SCALE;
		}
	}

	ChannelTarget getTarget() const { return target; }
	Path getPath() const { return path; }
	unsigned int getSamplerIndex() const { return samplerIndex; }
};

};

#endif /* CHANNEL_H */
