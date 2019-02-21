#ifndef GLYPH_H
#define GLYPH_H

#include <glm/glm.hpp>

class Glyph
{
    glm::ivec2 size;    
    glm::ivec2 bearing;    // Offset from baseline to left/top of glyph
    long int advance;    // Offset to advance to next glyph
public:
	Glyph(const glm::ivec2 &size, const glm::ivec2 &bearing, long int advance)
	{
		this->size = size;
		this->bearing = bearing;
		this->advance = advance;
	}
};

#endif /* GLYPH_H */
