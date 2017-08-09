#include "core/components/positioncomponent.h"
#include "core/components/spritecomponent.h"
#include "core/componentstore.h"
#include "video/systems/rendersystem.h"
#include "core/boiler.h"

void RenderSystem::update(ComponentStore &store, const double delta) const
{
	Boiler::getInstance().getRenderer().beginRender();
	for (auto &entity : getEntities())
	{
		std::shared_ptr<PositionComponent> pos = store.retrieve<PositionComponent>(entity);
		std::shared_ptr<SpriteComponent> sprite = store.retrieve<SpriteComponent>(entity);

		Boiler::getInstance().getRenderer().render(*pos, *sprite);
	}
	Boiler::getInstance().getRenderer().endRender();
}
