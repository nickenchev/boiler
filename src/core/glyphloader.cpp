#include <algorithm>
#include <boiler.h>
#include <video/opengl.h>
#include <video/opengltexture.h>
#include "core/glyphloader.h"
#include "video/glyphmap.h"
#include "video/glyph.h"
#include "video/vertexdata.h"
#include "video/model.h"
#include "util/texutil.h"

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

    const std::string characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ`1234567890-=~!@#$%^&*()_+[]{}\\|;:'\",<.>/? ";
    const unsigned long glyphCount = characters.length();
	std::vector<std::tuple<unsigned long, FT_Glyph>> glyphs;

	for (char c : characters)
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
                if (FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, nullptr, true) == 0)
                {
                    glyphs.push_back(std::make_tuple(c, glyph));
                }
                else
                {
                    logger.error("Error converting glyph to bitmap");
                }
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
	Size atlasSize(0, tallestGlyph->bitmap.rows);
	for (auto gtuple : glyphs)
	{
		FT_BitmapGlyph bmg = (FT_BitmapGlyph)std::get<1>(gtuple);
		atlasSize.width += bmg->bitmap.width;
	}

	// let opengl store textures that are not multiple of 4
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	// create opengl texture and setup params
	GLuint texture = 0;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlasSize.width, atlasSize.height, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
	if (glGetError() != GL_NO_ERROR)
	{
		logger.error("Unable to allocate glyph atlas texture.");
	}

	GLint swizzleMask[] = { GL_ONE, GL_ONE, GL_ONE, GL_RED };
	glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	std::unordered_map<unsigned long, const Glyph> glyphMap(glyphCount);

	GLfloat xOffset = 0;
	GLfloat yOffset = 0;
	float x = 0;
	float y = 0;
	for (auto tgl : glyphs)
	{
		unsigned long code = std::get<0>(tgl);
		FT_Glyph ftGlyph = std::get<1>(tgl);
		FT_BitmapGlyph bmg = (FT_BitmapGlyph)ftGlyph;
		Rect destRect(xOffset, yOffset, bmg->bitmap.width, bmg->bitmap.rows);
		glm::vec2 bearing(bmg->left, bmg->top);

		// write glyph to texture atlas
		glTexSubImage2D(GL_TEXTURE_2D, 0, xOffset, yOffset, destRect.size.width, destRect.size.height, GL_RED, GL_UNSIGNED_BYTE, bmg->bitmap.buffer);
		if (glGetError() != GL_NO_ERROR)
		{
			logger.error("Unable to write sub texture for glyph atlas.");
		}
		xOffset += destRect.size.width;

		// create model data
		const float scale = 1.0f;
		GLfloat xpos = x + bearing.x * scale;
		GLfloat ypos = y - (destRect.size.height - bearing.y) * scale;

		GLfloat w = destRect.size.width * scale;
		GLfloat h = destRect.size.height * scale;

		VertexData vertData(
		{
			{ xpos, ypos + h, 0.0f },
			{ xpos, ypos, 0.0f },
			{ xpos + w, ypos, 0.0f },
			{ xpos, ypos + h, 0.0f },
			{ xpos + w, ypos, 0.0f },
			{ xpos + w, ypos + h, 0.0f }           
        });

		const auto texCoords = TextureUtil::getTextureCoords(atlasSize, destRect);

		// create a VBO to hold each frame's texture coords
		GLuint texCoordVbo = 0;
		glGenBuffers(1, &texCoordVbo);
		glBindBuffer(GL_ARRAY_BUFFER, texCoordVbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * texCoords.size(), &texCoords[0], GL_STATIC_DRAW);

		if (glGetError() != GL_NO_ERROR)
		{
			logger.error("Unable to create the texture coordinate VBO.");
		}

		Glyph glyph(Boiler::getInstance().getRenderer().loadModel(vertData), texCoordVbo, destRect,
					glm::ivec2(bearing.x, bearing.y), ftGlyph->advance.x);

		glyphMap.insert({code, glyph});

		FT_Done_Glyph(ftGlyph);
	}
	FT_Done_Face(face);

	logger.log("Generated font atlas with dimensions " + std::to_string(atlasSize.width) + "x" + std::to_string(atlasSize.height));

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
