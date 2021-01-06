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
	SamplerId samplerId;

public:
	Channel(ChannelTarget target, const std::string &path, SamplerId samplerId)
	{
		this->target = target;
		this->samplerId = samplerId;

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
	SamplerId getSamplerId() const { return samplerId; }
};

};

#endif /* CHANNEL_H */
