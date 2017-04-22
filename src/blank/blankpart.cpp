#include <iostream>
#include "blankpart.h"
#include "core/rect.h"

BlankPart::BlankPart() : logger("Playground Part")
{
}

void BlankPart::onCreate()
{
    Boiler::getInstance().getRenderer().setClearColor(Color3(0.8f, 0.8f, 1.0f));
    Boiler::getInstance().addKeyInputListener([](const KeyInputEvent &event)
	{
		if (event.keyCode == SDLK_ESCAPE)
		{
			Boiler::getInstance().quit();
		}
	});

	EntityWorld &world = Boiler::getInstance().getEntityWorld();
	Entity entity = world.createEntity();

	ComponentMapper &mapper = Boiler::getInstance().getComponentMapper();
}

void BlankPart::update()
{
}
