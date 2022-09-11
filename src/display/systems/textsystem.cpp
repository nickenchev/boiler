#include "display/systems/textsystem.h"
#include "display/materialgroup.h"
#include "display/renderer.h"
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
		// setup material group
		assert(glyphMap.materialId != Asset::NO_ASSET);
		std::vector<MaterialGroup> group(1);
		group[0].materialId = glyphMap.materialId;

		AssetId matrixId = renderer.addMatrix(transform.getMatrix());

		cgfloat xOffset = 0;
		for (char c : text.text)
		{
			const Glyph &glyph = glyphMap.getMap().at(c);
			//logger.log("{} - {}x{}", c, glyph.getRect().size.width, glyph.getRect().size.height);

			xOffset += glyph.getBearing().x;
			group[0].primitives.push_back(MaterialGroup::PrimitiveInstance(glyph.primitiveId, matrixId, vec3(xOffset, 0, 0)));
			xOffset += glyph.getAdvance() >> 6;
		}
		renderer.render(assetSet, frameInfo, group, RenderStage::UI);
	}
}
