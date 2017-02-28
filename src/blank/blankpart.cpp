#include <iostream>
#include "blankpart.h"

BlankPart::BlankPart() : logger("Test Part")
{
}

void BlankPart::onCreate()
{
    Engine::getInstance().getRenderer().setClearColor(Color3(0.8f, 0.8f, 1.0f));
    Engine::getInstance().addKeyInputListener([](const KeyInputEvent &event)
	{
		if (event.keyCode == SDLK_ESCAPE)
		{
			Engine::getInstance().quit();
		}
	});

	EntityWorld &world = Engine::getInstance().getEntityWorld();
	Entity testEntity1 = world.createEntity();
	Entity testEntity2 = world.createEntity();
	Entity testEntity3 = world.createEntity();
	Entity testEntity4 = world.createEntity();
	world.removeEntity(testEntity2);
	world.removeEntity(testEntity3);
	testEntity2 = world.createEntity();
	testEntity3 = world.createEntity();
}

void BlankPart::update()
{
}
