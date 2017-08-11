#include <iostream>
#include "blankpart.h"
#include "video/systems/rendersystem.h"
#include "core/components/positioncomponent.h"
#include "core/components/spritecomponent.h"
#include "gravitysystem.h"
#include "velocitycomponent.h"

BlankPart::BlankPart() : logger("Playground Part")
{
	spriteSheet = Boiler::getInstance().getSpriteLoader().loadSheet("data/terrain.json");
    Boiler::getInstance().getRenderer().setClearColor(Color3(0.2f, 0.2f, 0.4f));
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

	ecs.getComponentSystems().registerSystem<GravitySystem>()
		.expects<PositionComponent>(ecs.getComponentMapper())
		.expects<VelocityComponent>(ecs.getComponentMapper());

	// create our entity and setup its components
	int tileSize = 150;
	Entity entity = ecs.newEntity();
	auto pos = ecs.addComponent<PositionComponent>(entity, Rect(40, 20, tileSize, tileSize));
	pos->absolute = true;
	auto sprite = ecs.addComponent<SpriteComponent>(entity, pos->frame);
	sprite->spriteFrame = spriteSheet->getFrame("forest.png");
	ecs.addComponent<VelocityComponent>(entity);
}

void BlankPart::onStart()
{
}
