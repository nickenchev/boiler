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

	Entity entity = Boiler::getInstance().getEcs().entityWorld.createEntity();

	auto &mapper = Boiler::getInstance().getEcs().mapper;
	mapper.registerComponent<PositionComponent>();
	mapper.registerComponent<SpriteComponent>();

	Boiler::getInstance().getEcs().systems.createSystem<RenderSystem>()
		.expects(mapper.mask<SpriteComponent>())
		.expects(mapper.mask<PositionComponent>());

	mapper.addComponent<PositionComponent>(entity);
	mapper.addComponent<SpriteComponent>(entity);
}

void BlankPart::update()
{
}
