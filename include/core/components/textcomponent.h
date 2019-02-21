#ifndef TEXTCOMPONENT_H
#define TEXTCOMPONENT_H

#include <string>
#include "core/componenttype.h"

class GlyphMap;

struct TextComponent : public ComponentType<TextComponent>
{
	std::string text;
	GlyphMap *glyphMap;
};

#endif /* TEXTCOMPONENT_H */
