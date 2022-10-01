#include <algorithm>
#include <tuple>
#include <vector>
#include <boiler.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#include "core/glyphloader.h"
#include "display/glyphmap.h"
#include "display/glyph.h"
#include "display/vertexdata.h"
#include "display/model.h"
#include "util/texutil.h"

#include "display/imaging/stb.h"

using namespace Boiler;

int nextPow2(int value);

GlyphLoader::GlyphLoader(Renderer &renderer, AssetSet &assetSet)
	: renderer(renderer), assetSet(assetSet), logger("Glyph Loader")
{
}

AssetId GlyphLoader::loadFace(std::string fontPath, int fontSize)
{
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		logger.error("Could not initialize FreeType");
	}

	FT_Face face;
	if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
	{
		logger.error("Could not load font");
	}

	assert(face->charmap != nullptr);

	logger.log("Building glyph atlas for " + fontPath);

	FT_Set_Pixel_Sizes(face, 0, fontSize);

	const std::string characters = " !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
	const unsigned long glyphCount = characters.length();
	std::vector<std::pair<unsigned long, FT_Glyph>> glyphs;

	for (char c : characters)
	{

		if (FT_Load_Glyph(face, FT_Get_Char_Index(face, c), FT_LOAD_DEFAULT))
		{
			logger.error("Failed to load glyph");
		}
		else
		{
			FT_Glyph glyph = nullptr;
			if (FT_Get_Glyph(face->glyph, &glyph))
			{
				logger.error("Failed to extract glyph");
			}
			else
			{
				if (FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, nullptr, true) == 0)
				{
					glyph->advance = face->glyph->advance;
					glyphs.push_back(std::pair<unsigned long, FT_Glyph>(c, glyph));
				}
				else
				{
					logger.error("Error converting glyph to bitmap");
				}
			}
		}
	}

	//sort by bitmap height
	std::sort(glyphs.begin(), glyphs.end(), [](std::pair<unsigned long, FT_Glyph> t1, std::pair<unsigned long, FT_Glyph> t2) {
		FT_BitmapGlyph bmg1 = (FT_BitmapGlyph)t1.second;
		FT_BitmapGlyph bmg2 = (FT_BitmapGlyph)t2.second;
		return bmg1->bitmap.rows > bmg2->bitmap.rows;
	});

	// figure out atlas size
	FT_BitmapGlyph tallestGlyph = (FT_BitmapGlyph)std::get<1>(glyphs[0]);
	Size atlasSize(0, tallestGlyph->bitmap.rows);
	for (auto gtuple : glyphs)
	{
		FT_BitmapGlyph bmg = (FT_BitmapGlyph)std::get<1>(gtuple);
		atlasSize.width += bmg->bitmap.width;
	}

	// ensure atlas dimensions are powers of 2
	atlasSize.width = nextPow2(static_cast<int>(atlasSize.width));
	atlasSize.height = nextPow2(static_cast<int>(atlasSize.height));

	// generate a glyph atlas
	const short outputChannels = 1;
	size_t atlasByteSize = (atlasSize.width * atlasSize.height) * outputChannels;
	unsigned char *atlasBuffer = new unsigned char[atlasByteSize]();

	std::unordered_map<unsigned long, const Glyph> glyphMap(glyphCount);

	// generate atlas from individual glyphs
	unsigned int xOffset = 0;
	unsigned int yOffset = 0;
	float x = 0;
	float y = 0;
	for (auto tgl : glyphs)
	{
		unsigned long code = tgl.first;
		FT_Glyph ftGlyph = tgl.second;
		FT_BitmapGlyph bmg = (FT_BitmapGlyph)ftGlyph;
		Rect destRect(xOffset, yOffset, bmg->bitmap.width, bmg->bitmap.rows);
		glm::vec2 bearing(bmg->left, bmg->top);

		for (int r = 0; r < bmg->bitmap.rows; ++r)
		{
			for (int c = 0; c < bmg->bitmap.width; ++c)
			{
				atlasBuffer[r * static_cast<int>(atlasSize.width) + xOffset + c] = bmg->bitmap.buffer[r * bmg->bitmap.width + c];
			}
		}
		xOffset += destRect.size.width;

		// create model data
		const cgfloat scale = 1.0f;
		cgfloat sizeW = destRect.size.width * scale;
		cgfloat sizeH = destRect.size.height * scale;

		// generate vertex buffer
		const float texX = destRect.position.x / atlasSize.width;
		const float texY = destRect.position.y / atlasSize.height;
		const float texW = (destRect.position.x + destRect.size.width) / atlasSize.width;
		const float texH = (destRect.position.y + destRect.size.height) / atlasSize.height;

		// generate vertices and texture coordinates
		std::vector<Vertex> verts = {
			Vertex(vec3(0.0f, sizeH, 0.0f), vec2(texX, texH), vec4(1, 1, 1, 1)),
			Vertex(vec3(sizeW, 0.0f, 0.0f), vec2(texW, texY), vec4(1, 1, 1, 1)),
			Vertex(vec3(0.0f, 0.0f, 0.0f), vec2(texX, texY), vec4(1, 1, 1, 1)),
			Vertex(vec3(sizeW, sizeH, 0.0f), vec2(texW, texH), vec4(1, 1, 1, 1))
		};
		std::vector<glm::uint32_t> indices = { 0, 1, 2, 0, 3, 1 };

		VertexData vertexData(verts, indices);
		AssetId bufferId = renderer.loadPrimitive(vertexData);

		// create primitive
		Primitive primitive(bufferId, std::move(vertexData), vec3(0, 0, 0), vec3(sizeW, sizeH, 0));
		AssetId primitiveId = assetSet.primitives.add(std::move(primitive));
		glyphMap.insert({code, Glyph(code, destRect, bearing, ftGlyph->advance.x, primitiveId)});

		FT_Done_Glyph(ftGlyph);
	}
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	Material glyphMat;
	glyphMat.baseTexture = renderer.loadTexture(ImageData(atlasBuffer, atlasSize, 1, false), TextureType::FREETYPE_ATLAS);
	AssetId materialId = assetSet.materials.add(std::move(glyphMat));

	logger.log("Generated font atlas with dimensions {}x{} ", static_cast<int>(atlasSize.width), static_cast<int>(atlasSize.height));
	return assetSet.glyphs.add(GlyphMap(materialId, glyphMap));
}

int nextPow2(int value)
{
	int pow = 1;
	while (value > pow)
	{
		pow *= 2;
	}
	return pow;
}
