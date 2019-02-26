#ifndef TEXTCOMPONENT_H
#define TEXTCOMPONENT_H

#include <string>
#include <glm/glm.hpp>
#include "core/componenttype.h"

class GlyphMap;

struct TextComponent : public ComponentType<TextComponent>
{
	std::string text;
	glm::vec4 colour;
	GlyphMap *glyphMap;

	TextComponent()
	{
		this->colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	}

	TextComponent(std::string text, glm::vec4 colour, GlyphMap *glyphMap)
	{
		this->text = text;
		this->colour = colour;
		this->glyphMap = glyphMap;
	}
};

#endif /* TEXTCOMPONENT_H */
