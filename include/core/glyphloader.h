#ifndef GLYPHLOADER_H
#define GLYPHLOADER_H

#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include "core/logger.h"

namespace Boiler
{

class GlyphMap;
class Renderer;

class GlyphLoader
{
	FT_Library ft;
	Logger logger;
	const Renderer &renderer;
	
public:
    GlyphLoader(const Renderer &renderer);
	virtual ~GlyphLoader();

	const GlyphMap loadFace(std::string fontPath, int fontSize);
};

}

#endif /* GLYPHLOADER_H */
