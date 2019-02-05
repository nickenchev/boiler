#include <boiler.h>
#include <video/opengl.h>
#include "core/glyphloader.h"

GlyphLoader::GlyphLoader() : logger("Glyph Loader")
{
	if (FT_Init_FreeType(&ft))
	{
		logger.error("Could not initialize FreeType");
	}
}

void GlyphLoader::loadFace(std::string fontPath)
{
	FT_Face face;
	if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
	{
		logger.error("Could not load font");
	}
	FT_Set_Pixel_Sizes(face, 0, 48);

	constexpr unsigned long glyphCount = 128;
	std::vector<FT_Glyph> glyphs(glyphCount);

	for (unsigned long c = 0; c < glyphs.size(); ++c)
	{
		if (FT_Load_Glyph(face, c, FT_LOAD_DEFAULT))
		{
			logger.error("Failed to load glyph");
		}
		if (FT_Get_Glyph(face->glyph, &glyphs[c]))
		{
			logger.error("Failed to extract glyph");
		}
		else
		{
			FT_Glyph_To_Bitmap(&glyphs[c], FT_RENDER_MODE_NORMAL, nullptr, true);
		}
	}

	//sort by pixel area
	auto cmprs = [](FT_Glyph glyph1, FT_Glyph glyph2) {
		FT_BitmapGlyph bmg1 = (FT_BitmapGlyph)glyph1;
		FT_BitmapGlyph bmg2 = (FT_BitmapGlyph)glyph2;
		//int area1 = bmg1->bitmap.width * bmg1->bitmap.rows;
		//int area2 = bmg2->bitmap.width * bmg2->bitmap.rows;

		return bmg1->bitmap.rows < bmg2->bitmap.rows;
	};
	logger.log("Building glyph atlas");
	std::sort(glyphs.begin(), glyphs.end(), cmprs);
	std::reverse(glyphs.begin(), glyphs.end());

	FT_BitmapGlyph tallestGlyph = (FT_BitmapGlyph)glyphs[0];
	GLfloat atlasHeight = tallestGlyph->bitmap.rows;
	GLfloat atlasWidth = 0;
	for (FT_Glyph glyph : glyphs)
	{
		FT_BitmapGlyph bmg = (FT_BitmapGlyph)glyph;
		atlasWidth += bmg->bitmap.width;
	}

	// let opengl store textures that are not multiple of 4
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// create opengl texture
	GLuint texture = 0;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlasWidth, atlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

	// write bitmap data to atlas
	GLfloat xOffset = 0;
	GLfloat yOffset = 0;
	for (FT_Glyph glyph : glyphs)
	{
		FT_BitmapGlyph bmg = (FT_BitmapGlyph)glyph;
		glTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset, bmg->bitmap.width, bmg->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, bmg->bitmap.buffer);
		xOffset += bmg->bitmap.width;
	}

	GLint swizzleMask[] = { GL_ONE, GL_ONE, GL_ONE, GL_RED };
	glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	logger.log("Loaded font at: " + fontPath);

	// clean up FT resources
	FT_Done_Face(face);
	for (FT_Glyph glyph : glyphs)
	{
		FT_Done_Glyph(glyph);
	}

			/*
			GLuint texture = 0;
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
