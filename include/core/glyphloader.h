#ifndef GLYPHLOADER_H
#define GLYPHLOADER_H

#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include "core/logger.h"

class GlyphMap;

class GlyphLoader
{
	FT_Library ft;
	Logger logger;
	
public:
    GlyphLoader();
	virtual ~GlyphLoader();

	const GlyphMap loadFace(std::string fontPath);
};


#endif /* GLYPHLOADER_H */
