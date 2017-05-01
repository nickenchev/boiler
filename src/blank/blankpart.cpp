#include <iostream>
#include "blankpart.h"
#include "video/systems/rendersystem.h"
#include "core/components/positioncomponent.h"
#include "core/components/spritecomponent.h"

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

	EntityComponentSystem &ecs = Boiler::getInstance().getEcs();
	ecs.getComponentMapper().registerComponent<PositionComponent>();
	ecs.getComponentMapper().registerComponent<SpriteComponent>();

	ecs.getComponentSystems().createSystem<RenderSystem>()
		.expects<PositionComponent>(ecs.getComponentMapper())
		.expects<SpriteComponent>(ecs.getComponentMapper());

	Entity entity = ecs.newEntity();
	ecs.addComponent<PositionComponent>(entity);
	ecs.addComponent<SpriteComponent>(entity);
}

void BlankPart::update()
{
}
