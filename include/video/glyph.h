#ifndef GLYPH_H
#define GLYPH_H

#include <memory>
#include <glm/glm.hpp>
#include "core/rect.h"

class Model;

class Glyph
{
    Size size;
    glm::ivec2 bearing;    // Offset from baseline to left/top of glyph
    long int advance;    // Offset to advance to next glyph
	std::shared_ptr<const Model> model;
public:
	Glyph(std::shared_ptr<const Model> model, const Size &size, const glm::ivec2 &bearing, long int advance) : model(model)
	{
		this->size = size;
		this->bearing = bearing;
		this->advance = advance;
	}
};

#endif /* GLYPH_H */
