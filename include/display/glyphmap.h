#ifndef GLYPHMAP_H
#define GLYPHMAP_H

#include <unordered_map>
#include "display/glyph.h"
#include "display/texture.h"
#include "display/imaging/imagedata.h"

namespace Boiler
{

class GlyphMap
{
	const ImageData imageData;
	std::unordered_map<unsigned long, const Glyph> map;

public:
    GlyphMap(ImageData &&imageData, const std::unordered_map<unsigned long, const Glyph> &map) : map(map), imageData(std::move(imageData)) {}
	~GlyphMap();

	const ImageData &getImageData() const { return imageData; }
	auto &getMap() const { return map; }
	const Glyph &operator[](unsigned long code) const
	{
		return map.at(code);
	}
};

}

#endif /* GLYPHMAP_H */
