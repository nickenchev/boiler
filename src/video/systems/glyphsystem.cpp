#include "video/systems/glyphsystem.h"
#include "core/components/positioncomponent.h"
#include "core/components/textcomponent.h"
#include "core/components/parentcomponent.h"
#include "video/renderer.h"
#include "core/engine.h"

// HACK - Temporary to get glyphs rendering, will use TextureInfo after refactor
#include "video/opengltextureinfo.h"

using namespace Boiler;

void GlyphSystem::update(ComponentStore &store, const double delta)
{
	for (auto &entity : getEntities())
	{
		PositionComponent &pos = store.retrieve<PositionComponent>(entity);
		TextComponent &text = store.retrieve<TextComponent>(entity);

		assert(text.glyphMap != nullptr);

		glm::mat4 modelMatrix = pos.getMatrix();

		Entity currentEntity = entity;
		while (store.hasComponent<ParentComponent>(currentEntity))
		{
			ParentComponent &parentComp = store.retrieve<ParentComponent>(currentEntity);
			PositionComponent &parentPos = store.retrieve<PositionComponent>(parentComp.entity);
			modelMatrix = parentPos.getMatrix() * modelMatrix;
			currentEntity = parentComp.entity;
		}

		const GlyphMap &glyphMap = *text.glyphMap;
		float xOffset = 0;
		for (char character : text.text)
		{
			const Glyph &glyph = glyphMap[static_cast<unsigned long>(character)];

			cgfloat x = xOffset + glyph.getBearing().x;
			cgfloat y = -glyph.getBearing().y;
			modelMatrix = glm::translate(modelMatrix, glm::vec3(x, y, 0));

			renderer.render(modelMatrix, glyph.getModel(), glyphMap.getSourceTexture(), glyph.getTextureInfo().get(), text.colour);

			// move to the next character position
			xOffset += (glyph.getAdvance() >> 6);
		}
	}
}
