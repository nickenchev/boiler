#ifndef TEXTCOMPONENT_H
#define TEXTCOMPONENT_H

#include <string>
#include "core/math.h"
#include "core/componenttype.h"
#include "display/glyphmap.h"
#include "display/glyph.h"

namespace Boiler
{

class GlyphMap;

struct TextComponent : public ComponentType<TextComponent>
{
	std::string text;
	vec4 colour;
	AssetId glyphId;

	TextComponent(const std::string &text, const vec4 &colour, AssetId glyphId) : ComponentType(this)
	{
		this->text = text;
		this->colour = colour;
		this->glyphId = glyphId;
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

}

#endif /* TEXTCOMPONENT_H */
