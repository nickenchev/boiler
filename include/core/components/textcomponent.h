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
	AssetId glyphId;

	TextComponent(const std::string &text, AssetId glyphId) : ComponentType(this)
	{
		this->text = text;
		this->glyphId = glyphId;
	}
};

}

#endif /* TEXTCOMPONENT_H */
