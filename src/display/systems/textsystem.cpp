#include "display/systems/textsystem.h"
#include "display/materialgroup.h"
#include "display/renderer.h"
#include "core/components/transformcomponent.h"
#include "core/components/textcomponent.h"
#include "core/entitycomponentsystem.h"

using namespace Boiler;

TextSystem::TextSystem() : System("Text System")
{
	expects<TransformComponent>();
	expects<TextComponent>();
}

void TextSystem::update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, EntityComponentSystem &ecs)
{
	for (Entity entity : getEntities())
	{
        TransformComponent &transform = ecs.getComponentStore().retrieve<TransformComponent>(entity);
        TextComponent &text = ecs.getComponentStore().retrieve<TextComponent>(entity);

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
			cgfloat height = glyph.getRect().size.height;
			cgfloat bearY = glyph.getBearing().y;
			//logger.log("{} - H:{} Y:{}", c, glyph.getRect().size.height, glyph.getBearing().y);

			vec3 offset(xOffset, -height - (bearY - height), 0);
			group[0].primitives.push_back(MaterialGroup::PrimitiveInstance(glyph.primitiveId, matrixId, offset));

			xOffset += (glyph.getAdvance() >> 6);
		}
		renderer.render(assetSet, frameInfo, group, RenderStage::UI);
	}
}
