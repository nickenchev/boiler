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
	Entity entity = ecs.newEntity();

	ecs.getComponentMapper().addComponent<PositionComponent>(entity);
	ecs.getComponentMapper().addComponent<SpriteComponent>(entity);

	ecs.getComponentSystems().createSystem<RenderSystem>()
		.expects<PositionComponent>(ecs.getComponentMapper())
		.expects<SpriteComponent>(ecs.getComponentMapper());
}

void BlankPart::update()
{
}
