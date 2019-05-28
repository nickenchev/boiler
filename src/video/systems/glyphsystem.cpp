#include "video/systems/glyphsystem.h"
#include "core/components/positioncomponent.h"
#include "core/components/textcomponent.h"
#include "video/renderer.h"
#include "core/engine.h"

using namespace Boiler;

void GlyphSystem::update(ComponentStore &store, const double delta)
{
	for (auto &entity : getEntities())
	{
		PositionComponent pos = store.retrieve<PositionComponent>(entity);
		TextComponent &text = store.retrieve<TextComponent>(entity);

		assert(text.glyphMap != nullptr);

		// offset by parent's position
		if (pos.parent)
		{
			pos.frame.position += pos.parent->frame.position;
		}

		renderer.render(pos, text);
	}
}
