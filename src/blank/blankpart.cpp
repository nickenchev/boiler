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

    Boiler::getInstance().getRenderer().setClearColor(Color3(0.8f, 0.8f, 1.0f));
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
	Entity entity = ecs.newEntity();
    auto pos = ecs.addComponent<PositionComponent>(entity, Rect(10, 10, 100, 100));
	pos->absolute = true;

	auto sprite = ecs.addComponent<SpriteComponent>(entity, pos->frame);
	sprite->spriteFrame = spriteSheet->getFrame("forest.png");
}
