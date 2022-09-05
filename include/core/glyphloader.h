#ifndef GLYPHLOADER_H
#define GLYPHLOADER_H

#include <string>
#include "core/logger.h"
#include "display/glyphmap.h"

namespace Boiler
{

class Renderer;

class GlyphLoader
{
	Logger logger;
	
public:
    GlyphLoader();

	const GlyphMap loadFace(std::string fontPath, int fontSize);
};

}

#endif /* GLYPHLOADER_H */
