#ifndef FRAMEINFO_H
#define FRAMEINFO_H

#include <array>
#include <chrono>
#include "core/common.h"
#include "input/keyinputevent.h"

namespace Boiler
{

struct FrameInfo
{
	short currentFrame;
	double globalTime;
	std::chrono::high_resolution_clock::time_point currentTime;
	Time frameTime;
	Time deltaTime;
	unsigned long frameCount;

	FrameInfo()
	{
		currentFrame = 0;
		deltaTime = 0;
		frameTime = 0;
		globalTime = 0;
		frameCount = 0;
	}
};

}

#endif /* FRAMEINFO_H */
