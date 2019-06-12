#ifndef GLYPH_H
#define GLYPH_H

#include <memory>
#include <glm/glm.hpp>
#include "core/rect.h"
#include "video/opengltextureinfo.h"

namespace Boiler
{

class Model;

class Glyph
{
	unsigned long code;
	Rect sourceRect;
    glm::ivec2 bearing;    // Offset from baseline to left/top of glyph
    long int advance;    // Offset to advance to next glyph
	std::shared_ptr<const Model> model;
	const OpenGLTextureInfo textureInfo;
	
public:
	Glyph(unsigned long code, std::shared_ptr<const Model> model,
		  unsigned int texCoordsVbo, const Rect &sourceRect,
		  const glm::ivec2 &bearing, long int advance) : model(model), textureInfo(texCoordsVbo)
	{
		this->code = code;
		this->sourceRect = sourceRect;
		this->bearing = bearing;
		this->advance = advance;
	}

	auto getModel() const { return model; }
	const OpenGLTextureInfo &getTextureInfo() const { return textureInfo; }
	auto &getBearing() const { return bearing; }
	auto &getAdvance() const { return advance; }
};

}

#endif /* GLYPH_H */
