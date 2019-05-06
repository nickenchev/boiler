#ifndef TEXUTIL_H
#define TEXUTIL_H

#include <vector>
#include "core/rect.h"

namespace Boiler
{

class TextureUtil
{
public:
    TextureUtil() = delete;

	static auto getTextureCoords(const Size &sheetSize, const Rect &portion)
	{
		// calculate the opengl texture coords
		const float texX = portion.position.x / sheetSize.width;
		const float texY = portion.position.y / sheetSize.height;
		const float texW = (portion.position.x + portion.size.width) / sheetSize.width;
		const float texH = (portion.position.y + portion.size.height) / sheetSize.height;

		std::vector<float> texCoords(
		{
			texX, texH,
			texW, texY,
			texX, texY,

			texX, texH,
			texW, texH,
			texW, texY
		});

		return texCoords;
	}
};

}

#endif /* TEXUTIL_H */
