#include "core/components/positioncomponent.h"
#include "core/components/spritecomponent.h"
#include "core/componentstore.h"
#include "video/systems/rendersystem.h"

using namespace Boiler;

void RenderSystem::update(ComponentStore &store, const double delta)
{
	for (auto &entity : getEntities())
	{
		PositionComponent &pos = store.retrieve<PositionComponent>(entity);
		SpriteComponent &sprite = store.retrieve<SpriteComponent>(entity);

		renderer.render(pos, sprite);
	}
}
