#include <iostream>
#include "blankpart.h"
#include "core/rect.h"
#include "core/component.h"

BlankPart::BlankPart() : logger("Playground Part")
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
	Entity entity = world.createEntity();

	ComponentMapper &mapper = Engine::getInstance().getComponentMapper();
	Component<Rect> positionComponent;
}

void BlankPart::update()
{
}
