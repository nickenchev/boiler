#ifndef GLYPH_H
#define GLYPH_H

#include <memory>
#include "core/math.h"
#include "core/rect.h"
#include "video/textureinfo.h"
#include "video/model.h"

namespace Boiler
{

class Model;

class Glyph
{
	unsigned long code;
	Rect sourceRect;
    ivec2 bearing;    // Offset from baseline to left/top of glyph
    long int advance;    // Offset to advance to next glyph
	Model model;
	std::shared_ptr<TextureInfo> textureInfo;
	
public:
	Glyph(unsigned long code, const Model &model, std::shared_ptr<TextureInfo> textureInfo, const Rect &sourceRect,
		  const ivec2 &bearing, long int advance) : model(model)
	{
		this->code = code;
		this->sourceRect = sourceRect;
		this->bearing = bearing;
		this->advance = advance;
		this->textureInfo = textureInfo;
	}

	auto &getBearing() const { return bearing; }
	auto &getAdvance() const { return advance; }
	auto getModel() const { return model; }
	auto getTextureInfo() const { return textureInfo; }
};

}

#endif /* GLYPH_H */
