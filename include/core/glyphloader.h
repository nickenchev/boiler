#ifndef GLYPHLOADER_H
#define GLYPHLOADER_H

#include <string>
#include "core/logger.h"
#include "display/glyphmap.h"

namespace Boiler
{

class AssetSet;
class Renderer;

class GlyphLoader
{
	Renderer &renderer;
	AssetSet &assetSet;
	Logger logger;
	
public:
    GlyphLoader(Renderer &renderer, AssetSet &assetSet);

	AssetId loadFace(std::string fontPath, int fontSize);
};

}

#endif /* GLYPHLOADER_H */
