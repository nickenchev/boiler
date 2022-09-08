#include "display/systems/textsystem.h"
#include "core/components/transformcomponent.h"
#include "core/components/textcomponent.h"
#include "core/componentstore.h"

using namespace Boiler;

TextSystem::TextSystem() : System("Text System")
{
	expects<TransformComponent>();
	expects<TextComponent>();
}

void TextSystem::update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, ComponentStore &store)
{
	for (Entity entity : getEntities())
	{
		TransformComponent &transform = store.retrieve<TransformComponent>(entity);
		TextComponent &text = store.retrieve<TextComponent>(entity);

		const GlyphMap &glyphMap = assetSet.glyphs.get(text.glyphId);

		for (char c : text.text)
		{
			const Glyph &glyph = glyphMap.getMap().at(c);
			logger.log("{}", glyph.primitiveId);
		}
	}
}
