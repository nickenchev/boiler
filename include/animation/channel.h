#ifndef CHANNEL_H
#define CHANNEL_H

#include <string>
#include <vector>
#include "core/entity.h"
#include "animationsampler.h"

namespace Boiler
{

enum class Path
{
	TRANSLATION,
	ROTATION,
	SCALE,
};

class Channel
{
	Entity entity;
	Path path;
	unsigned int samplerIndex;

public:
	Channel(Entity entity, const std::string &path, unsigned int samplerIndex)
	{
		this->entity = entity;
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
		else
		{
			exit(1);
		}
	}

	Entity getEntity() const { return entity; }
	Path getPath() const { return path; }
	unsigned int getSamplerIndex() const { return samplerIndex; }
};

};

#endif /* CHANNEL_H */
