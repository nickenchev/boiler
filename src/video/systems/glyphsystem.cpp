#include "video/systems/glyphsystem.h"
#include "core/components/positioncomponent.h"
#include "core/components/spritecomponent.h"
#include "video/renderer.h"
#include "core/boiler.h"

void GlyphSystem::update(ComponentStore &store, const double delta) const
{
	for (auto &entity : getEntities())
	{
		//PositionComponent &pos = store.retrieve<PositionComponent>(entity);
		//SpriteComponent &sprite = store.retrieve<SpriteComponent>(entity);

		//Boiler::getInstance().getRenderer().render(pos, sprite);
	}
}
