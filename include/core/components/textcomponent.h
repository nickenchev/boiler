#ifndef TEXTCOMPONENT_H
#define TEXTCOMPONENT_H

#include <string>
#include <glm/glm.hpp>
#include "core/componenttype.h"
#include "video/glyphmap.h"
#include "video/glyph.h"

class GlyphMap;

struct TextComponent : public ComponentType<TextComponent>
{
	std::string text;
	glm::vec4 colour;
	const GlyphMap *glyphMap;

	TextComponent(const std::string &text, const glm::vec4 &colour, const GlyphMap *glyphMap)
	{
		this->text = text;
		this->colour = colour;
		this->glyphMap = glyphMap;
		/*
		const float scale = 1;
		for (char c : text)
		{
			const Glyph &glyph = glyphMap[c];
			float xpos = position.frame.position.x + glyph.bearing.x * scale;
			float ypos = position.frame.position.y - (glyph.size.y - glyph.bearing.y) * scale;

			float w = glyph.size.x * scale;
			float h = glyph.size.y * scale;

			VertexData vertData(
			{
				{ xpos,     ypos + h, 0 },
				{ xpos,     ypos, 0 },
				{ xpos + w, ypos, 0 },

				{ xpos,     ypos + h, 0 },
				{ xpos + w, ypos, 0 },
				{ xpos + w, ypos + h, 0 }           
			});
		}
		model = Boiler::getInstance().getRenderer().loadModel(vertData);
		*/
	}
};

#endif /* TEXTCOMPONENT_H */
