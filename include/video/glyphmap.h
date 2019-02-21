#ifndef GLYPHMAP_H
#define GLYPHMAP_H

#include <unordered_map>

class Glyph;
class Texture;

class GlyphMap
{
    const std::shared_ptr<const Texture> sourceTexture;
	std::unordered_map<unsigned long, const Glyph> map;
public:
    GlyphMap(const std::shared_ptr<const Texture> &sourceTexture,
			 const std::unordered_map<unsigned long, const Glyph> &map) : sourceTexture(sourceTexture), map(map) { }
};


#endif /* GLYPHMAP_H */
