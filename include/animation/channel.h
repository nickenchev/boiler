#ifndef CHANNEL_H
#define CHANNEL_H

#include <string>
#include <vector>
#include "core/entity.h"
#include "animationsampler.h"

namespace Boiler
{

class Channel
{
	Entity entity;
	std::string path;
	unsigned int samplerIndex;

public:
	Channel(Entity entity, const std::string &path, unsigned int samplerIndex)
	{
		this->entity = entity;
		this->path = path;
		this->samplerIndex = samplerIndex;
	}

	Entity getEntity() const { return entity; }
	std::string getPath() const { return path; }
	unsigned int getSamplerIndex() const { return samplerIndex; }
};

};

#endif /* CHANNEL_H */
