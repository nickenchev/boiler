#ifndef GLYPH_H
#define GLYPH_H

#include "core/math.h"
#include "core/rect.h"
#include "display/primitive.h"

namespace Boiler
{

class Model;

class Glyph
{
	unsigned long code;
	Rect sourceRect;
    ivec2 bearing;    // Offset from baseline to left/top of glyph
    long int advance;    // Offset to advance to next glyph
	
public:
	AssetId primitiveId;

	Glyph(unsigned long code, const Rect &sourceRect, const ivec2 &bearing, long int advance, AssetId primitiveId)
	{
		this->code = code;
		this->sourceRect = sourceRect;
		this->bearing = bearing;
		this->advance = advance;
		this->primitiveId = primitiveId;
	}

	auto &getRect() const { return sourceRect; }
	auto &getBearing() const { return bearing; }
	auto &getAdvance() const { return advance; }

	VertexData vertexData() const
	{
		cgfloat sizeW = sourceRect.size.width;
		cgfloat sizeH = sourceRect.size.height;

		std::vector<Vertex> verts = {
			Vertex(vec3(0.0f, sizeH, 0.0f)),
			Vertex(vec3(sizeW, 0.0f, 0.0f)),
			Vertex(vec3(0.0f, 0.0f, 0.0f)),
			Vertex(vec3(sizeW, sizeH, 0.0f))
		};
		std::vector<glm::uint32_t> indices = { 0, 1, 2, 0, 3, 1 };

		return VertexData(verts, indices);
	}
};

}

#endif /* GLYPH_H */
