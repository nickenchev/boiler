#ifndef GLYPHMAP_H
#define GLYPHMAP_H

#include <unordered_map>
#include "video/glyph.h"

namespace Boiler
{

class Texture;

class GlyphMap
{
    const std::shared_ptr<const Texture> sourceTexture;
	std::unordered_map<unsigned long, const Glyph> map;
public:
    GlyphMap(const std::shared_ptr<const Texture> &sourceTexture,
			 const std::unordered_map<unsigned long, const Glyph> &map) : sourceTexture(sourceTexture), map(map) { }

	~GlyphMap();

	const Glyph &operator[](unsigned long code) const
	{
		return map.at(code);
	}
	auto getSourceTexture() const { return sourceTexture; }
};

}

#endif /* GLYPHMAP_H */
