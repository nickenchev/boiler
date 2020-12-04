#include "video/systems/glyphsystem.h"
#include "core/components/transformcomponent.h"
#include "core/components/textcomponent.h"
#include "core/components/parentcomponent.h"
#include "video/renderer.h"
#include "core/engine.h"

using namespace Boiler;

void GlyphSystem::update(ComponentStore &store, const double delta)
{
	for (auto &entity : getEntities())
	{
		TransformComponent &transform = store.retrieve<TransformComponent>(entity);
		TextComponent &text = store.retrieve<TextComponent>(entity);

		assert(text.glyphMap != nullptr);

		glm::mat4 modelMatrix = transform.getMatrix();

		Entity currentEntity = entity;
		while (store.hasComponent<ParentComponent>(currentEntity))
		{
			ParentComponent &parentComp = store.retrieve<ParentComponent>(currentEntity);
			TransformComponent &parentTransform = store.retrieve<TransformComponent>(parentComp.entity);
			modelMatrix = parentTransform.getMatrix() * modelMatrix;
			currentEntity = parentComp.entity;
		}

		const GlyphMap &glyphMap = *text.glyphMap;
		float xOffset = 0;
		for (char character : text.text)
		{
			const Glyph &glyph = glyphMap[static_cast<unsigned long>(character)];

			cgfloat x = xOffset + glyph.getBearing().x;
			cgfloat y = -glyph.getBearing().y;
			glm::mat4 glyphMatrix = modelMatrix * glm::translate(glm::mat4(1), glm::vec3(x, y, 0));

			//renderer.render(glyphMatrix, glyph.getPrimitive(), glyphMap.getSourceTexture(), glyph.getTextureInfo().get(), text.colour);

			// move to the next character position
			xOffset += (glyph.getAdvance() >> 6);
		}
	}
}
