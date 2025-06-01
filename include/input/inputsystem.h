#pragma once

#include <array>
#include "core/system.h"
#define KEY_STATES_SIZE 256

namespace Boiler
{

class Engine;

class KeyInputEvents
{
	unsigned int count;
	std::array<KeyInputEvent, 128> keyEvents;

public:
	KeyInputEvents()
	{
		reset();
	}

	void reset()
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

class InputSystem : public System
{
	KeyInputEvents keyEvents;
	ButtonState keyStates[KEY_STATES_SIZE];
	Engine &engine;
	bool moveLeft, moveRight, moveForward, moveBackward, moveUp, moveDown;
	float prevXFactor, prevYFactor;
	int mouseXDiff, mouseYDiff;
	
public:
    InputSystem(Engine &engine);
    void update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, EntityComponentSystem &ecs) override;

	void addKeyEvent(const KeyInputEvent &event);
	int getMouseXDiff() const { return mouseXDiff; }
	void setMouseXDiff(int xDiff) { mouseXDiff = xDiff; }
	int getMouseYDiff() const { return mouseYDiff; }
	void setMouseYDiff(int yDiff) { mouseYDiff = yDiff; }
};

}
