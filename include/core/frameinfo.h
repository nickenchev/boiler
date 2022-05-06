#ifndef FRAMEINFO_H
#define FRAMEINFO_H

#include <array>
#include "core/common.h"
#include "input/inputevent.h"

namespace Boiler
{

constexpr size_t maxInputEvents = 32;

struct FrameInfo
{
	std::array<InputEvent, maxInputEvents> inputEvents;
	short currentFrame;
	Time deltaTime, globalTime;
};

}

#endif /* FRAMEINFO_H */
