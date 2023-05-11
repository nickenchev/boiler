#ifndef GLYPHMAP_H
#define GLYPHMAP_H

#include <unordered_map>
#include "display/glyph.h"

namespace Boiler
{

class GlyphMap
{
	std::unordered_map<unsigned long, const Glyph> map;

public:
	AssetId materialId;

	GlyphMap()
	{
	}
	
    GlyphMap(AssetId materialId, const std::unordered_map<unsigned long, const Glyph> &map) : materialId(materialId), map(std::move(map))
	{
	}

	~GlyphMap();

	GlyphMap &operator=(GlyphMap &&glyphMap)
	{
		this->materialId = glyphMap.materialId;
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
