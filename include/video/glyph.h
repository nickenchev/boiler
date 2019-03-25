#ifndef GLYPH_H
#define GLYPH_H

#include <memory>
#include <glm/glm.hpp>
#include "core/rect.h"

class Model;

class Glyph
{
	unsigned long code;
	Rect sourceRect;
    glm::ivec2 bearing;    // Offset from baseline to left/top of glyph
    long int advance;    // Offset to advance to next glyph
	std::shared_ptr<const Model> model;
    unsigned int texCoordsVbo;
	
public:
	Glyph(unsigned long code, std::shared_ptr<const Model> model, unsigned int texCoordsVbo, const Rect &sourceRect, const glm::ivec2 &bearing, long int advance) : model(model)
	{
		this->code = code;
		this->sourceRect = sourceRect;
		this->bearing = bearing;
		this->advance = advance;
		this->texCoordsVbo = texCoordsVbo;
	}

	auto getModel() const { return model; }
    unsigned int getTexCoordsVbo() const { return texCoordsVbo; }
	auto &getBearing() const { return bearing; }
	auto &getAdvance() const { return advance; }
};

#endif /* GLYPH_H */
