#include "video/systems/glyphsystem.h"
#include "core/components/positioncomponent.h"
#include "core/components/textcomponent.h"
#include "video/renderer.h"
#include "core/engine.h"

using namespace Boiler;

void GlyphSystem::update(ComponentStore &store, const double delta) const
{
	for (auto &entity : getEntities())
	{
		PositionComponent pos = store.retrieve<PositionComponent>(entity);
		TextComponent &text = store.retrieve<TextComponent>(entity);

		// offset by parent's position
		if (pos.parent)
		{
			pos.frame.position += pos.parent->frame.position;
		}

		assert(text.glyphMap != nullptr);
		Engine::getInstance().getRenderer().render(pos, text);
	}
}
