#include <algorithm>
#include <boiler.h>
#include <video/opengl.h>
#include <video/opengltexture.h>
#include "core/glyphloader.h"
#include "video/glyphmap.h"

GlyphLoader::GlyphLoader() : logger("Glyph Loader")
{
	if (FT_Init_FreeType(&ft))
	{
		logger.error("Could not initialize FreeType");
	}
}

const GlyphMap GlyphLoader::loadFace(std::string fontPath)
{
	FT_Face face;
	if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
	{
		logger.error("Could not load font");
	}
	FT_Set_Pixel_Sizes(face, 0, 48);

	constexpr unsigned long glyphCount = 128;
	//std::vector<FT_Glyph> glyphs(glyphCount);
	std::vector<std::tuple<unsigned long, FT_Glyph>> glyphs;

	for (unsigned long c = 0; c < glyphCount; ++c)
	{
		if (FT_Load_Glyph(face, c, FT_LOAD_DEFAULT))
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
				FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, nullptr, true);
				glyphs.push_back(std::make_tuple(c, glyph));
			}
		}
	}

	//sort by pixel area
	auto cmprs = [](std::tuple<unsigned long, FT_Glyph> t1, std::tuple<unsigned long, FT_Glyph> t2) {
		FT_BitmapGlyph bmg1 = (FT_BitmapGlyph)std::get<1>(t1);
		FT_BitmapGlyph bmg2 = (FT_BitmapGlyph)std::get<1>(t2);
		return bmg1->bitmap.rows < bmg2->bitmap.rows;
	};
	logger.log("Building glyph atlas for " + fontPath);
	std::sort(glyphs.begin(), glyphs.end(), cmprs);
	std::reverse(glyphs.begin(), glyphs.end());

	// figure out atlas size
	FT_BitmapGlyph tallestGlyph = (FT_BitmapGlyph)std::get<1>(glyphs[0]);
	GLfloat atlasHeight = tallestGlyph->bitmap.rows;
	GLfloat atlasWidth = 0;
	for (auto gtuple : glyphs)
	{
		FT_BitmapGlyph bmg = (FT_BitmapGlyph)std::get<1>(gtuple);
		atlasWidth += bmg->bitmap.width;
	}

	// let opengl store textures that are not multiple of 4
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// create opengl texture and setup params
	GLuint texture = 0;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlasWidth, atlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
	GLint swizzleMask[] = { GL_ONE, GL_ONE, GL_ONE, GL_RED };
	glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	std::unordered_map<unsigned long, const Glyph> glyphMap(glyphCount);

	// write bitmap data to atlas
	GLfloat xOffset = 0;
	GLfloat yOffset = 0;
	for (auto tgl : glyphs)
	{
		unsigned long code = std::get<0>(tgl);
		FT_Glyph ftGlyph = std::get<1>(tgl);
		FT_BitmapGlyph bmg = (FT_BitmapGlyph)ftGlyph;
		glTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset, bmg->bitmap.width, bmg->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, bmg->bitmap.buffer);
		xOffset += bmg->bitmap.width;

		Glyph glyph(glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
					glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
					face->glyph->advance.x);

		glyphMap.insert({code, glyph});

		FT_Done_Glyph(ftGlyph);
	}
	FT_Done_Face(face);

	logger.log("Generated font atlas with dimensions " + std::to_string(atlasWidth) + "x" + std::to_string(atlasHeight));

	return GlyphMap(std::make_shared<OpenGLTexture>(fontPath, texture), glyphMap);

			/*
			glyph texture = 0;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

			Glyph glyph = {
				texture,
				glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
				glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
				face->glyph->advance.x
			};
			glyphs.insert(std::pair<GLchar, Glyph>(c, glyph));
			*/
}

GlyphLoader::~GlyphLoader()
{
	FT_Done_FreeType(ft);
}
