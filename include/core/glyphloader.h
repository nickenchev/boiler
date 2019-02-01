#ifndef GLYPHLOADER_H
#define GLYPHLOADER_H

#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include "core/rect.h"
#include "core/logger.h"
#include "video/glyph.h"

class GlyphLoader
{
	FT_Library ft;
	Logger logger;
	
public:
    GlyphLoader();
	virtual ~GlyphLoader();

	void loadFace(std::string fontPath);
};


#endif /* GLYPHLOADER_H */
