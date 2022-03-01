#ifndef FRAMEINFO_H
#define FRAMEINFO_H

#include "core/common.h"

namespace Boiler
{

class FrameInfo
{
	short currentFrame;
	Time deltaTime, globalTime;

public:
	FrameInfo(short currentFrame, Time deltaTime, Time globalTime)
	{
		this->currentFrame = currentFrame;
		this->deltaTime = deltaTime;
		this->globalTime = globalTime;
	}

	short getCurrentFrame() const { return currentFrame; }
	Time getDeltaTime() const { return deltaTime; }
	Time getGlobalTime() const { return globalTime; }
};

}

#endif /* FRAMEINFO_H */
