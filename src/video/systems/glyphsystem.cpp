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
		PositionComponent &pos = store.retrieve<PositionComponent>(entity);
		TextComponent &text = store.retrieve<TextComponent>(entity);

		assert(text.glyphMap != nullptr);

		//renderer.render(pos, text);
	}
}
