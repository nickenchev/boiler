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
	std::vector<MaterialGroup> groups;
	groups.reserve(5);

	for (Entity entity : getEntities())
	{
        TransformComponent &transform = ecs.getComponentStore().retrieve<TransformComponent>(entity);
        TextComponent &text = ecs.getComponentStore().retrieve<TextComponent>(entity);

		const GlyphMap &glyphMap = assetSet.glyphs.get(text.glyphId);
		// setup material group
		assert(glyphMap.materialId != Asset::NO_ASSET);

		// setup material group
		auto result = std::find_if(groups.begin(), groups.end(), [&glyphMap](const MaterialGroup &group) {
			return group.materialId == glyphMap.materialId;
		});

		MaterialGroup *group = nullptr;
		if (result != groups.end())
		{
			group = result.base();
		}
		else
		{
			groups.push_back(MaterialGroup(glyphMap.materialId));
			group = &groups[groups.size() - 1];
		}

		AssetId matrixId = renderer.addMatrix(transform.getMatrix());
		cgfloat xOffset = 0;
		for (char c : text.text)
		{
			const Glyph &glyph = glyphMap.getMap().at(c);
			cgfloat height = glyph.getRect().size.height;
			cgfloat bearY = glyph.getBearing().y;

			vec3 offset(xOffset, -height - (bearY - height), 0);
			group->primitives.push_back(MaterialGroup::PrimitiveInstance(glyph.primitiveId, matrixId, offset));

			xOffset += (glyph.getAdvance() >> 6);
		}
	}
	renderer.render(assetSet, frameInfo, groups, RenderStage::TEXT);
}
