#include <iostream>
#include "blankpart.h"
#include "video/systems/rendersystem.h"
#include "core/components/positioncomponent.h"
#include "core/components/spritecomponent.h"

BlankPart::BlankPart() : logger("Playground Part")
{
}

void BlankPart::onStart()
{
    spriteSheet = Boiler::getInstance().getSpriteLoader().loadSheet("data/terrain.json");

    Boiler::getInstance().getRenderer().setClearColor(Color3(0, 0, 0));
    Boiler::getInstance().addKeyInputListener([](const KeyInputEvent &event)
	{
		if (event.keyCode == SDLK_ESCAPE)
		{
			Boiler::getInstance().quit();
		}
	});

	EntityComponentSystem &ecs = Boiler::getInstance().getEcs();

	// add the rendering system
	ecs.getComponentSystems().registerSystem<RenderSystem>()
		.expects<PositionComponent>(ecs.getComponentMapper())
		.expects<SpriteComponent>(ecs.getComponentMapper());

	// create our entity and setup its components
	for (int r = 0; r < 10; ++r)
	{
		for (int c = 0; c < 10; ++c)
		{
			Entity entity = ecs.newEntity();
			auto pos = ecs.addComponent<PositionComponent>(entity, Rect(100 * r, 100 * c, 100, 100));
			pos->absolute = true;
			auto sprite = ecs.addComponent<SpriteComponent>(entity, pos->frame);
			sprite->spriteFrame = spriteSheet->getFrame("forest.png");
		}
	}
}
