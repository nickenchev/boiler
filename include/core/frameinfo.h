#ifndef FRAMEINFO_H
#define FRAMEINFO_H

#include <array>
#include "core/common.h"
#include "input/keyinputevent.h"

namespace Boiler
{

class KeyInputEvents
{
	unsigned int count;
	std::array<KeyInputEvent, 128> keyEvents;

public:
	KeyInputEvents()
	{
		count = 0;
	}
	void addEvent(KeyInputEvent event)
	{
		keyEvents[count++] = event;
	}
	unsigned int getCount() const
	{
		return count;
	}

	const KeyInputEvent &operator[](unsigned int index) const
	{
		return keyEvents[index];
	}
};

struct FrameInfo
{
	KeyInputEvents keyInputEvents;

	short currentFrame;
	double globalTime;
	Time deltaTime;
	int mouseXDistance, mouseYDistance;

	FrameInfo()
	{
		currentFrame = 0;
		deltaTime = 0;
		globalTime = 0;
		mouseXDistance = 0;
		mouseYDistance = 0;
	}
};

}

#endif /* FRAMEINFO_H */
