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
	AssetId textureId;
	std::unordered_map<unsigned long, const Glyph> map;

public:
	GlyphMap()
	{
		textureId = Asset::NO_ASSET;
	}
	
    GlyphMap(AssetId textureId, const std::unordered_map<unsigned long, const Glyph> &map) : textureId(textureId), map(std::move(map))
	{
	}

	~GlyphMap();

	GlyphMap &operator=(GlyphMap &&glyphMap)
	{
		this->textureId = glyphMap.textureId;
		this->map = std::move(glyphMap.map);

		return *this;
	}

	auto &getMap() const { return map; }
	const Glyph &operator[](unsigned long code) const
	{
		return map.at(code);
	}
};

}

#endif /* GLYPHMAP_H */
