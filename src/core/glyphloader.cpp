#include <algorithm>
#include <tuple>
#include <vector>
#include <boiler.h>
#include "core/glyphloader.h"
#include "display/glyphmap.h"
#include "display/glyph.h"
#include "display/vertexdata.h"
#include "display/model.h"
#include "util/texutil.h"

#include "display/imaging/stb.h"

using namespace Boiler;

GlyphLoader::GlyphLoader() : logger("Glyph Loader")
{
	if (FT_Init_FreeType(&ft))
	{
		logger.error("Could not initialize FreeType");
	}
}

const GlyphMap GlyphLoader::loadFace(std::string fontPath, int fontSize)
{
	FT_Face face;
	if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
	{
		logger.error("Could not load font");
	}
	else
	{
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
		// generate a glyph atlas
		size_t atlasByteSize = atlasSize.width * atlasSize.height;
		char *atlasBuffer = new char[atlasByteSize]();

		/*
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
		*/

		std::unordered_map<unsigned long, const Glyph> glyphMap(glyphCount);

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

			/*
			std::vector<Vertex> verts = {
				0.0f, sizeH, 0.0f,
				sizeW, 0.0f, 0.0f,
				0.0f, 0.0f, 0.0f,

				0.0f, sizeH, 0.0f,
				sizeW, sizeH, 0.0f,
				sizeW, 0.0f, 0.0f
			};
			VertexData vertData(verts);

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

			glyphMap.insert({code, Glyph(code, renderer.loadModel(vertData), std::make_shared<OpenGLTextureInfo>(texCoordVbo),
										destRect, glm::ivec2(bearing.x, bearing.y), ftGlyph->advance.x)});

			*/
			FT_Done_Glyph(ftGlyph);
		}
		stbi_write_bmp("test/atlas", static_cast<int>(atlasSize.width), static_cast<int>(atlasSize.height), 1, atlasBuffer);

		delete [] atlasBuffer;

		FT_Done_Face(face);

		logger.log("Generated font atlas with dimensions {}x{} ", static_cast<int>(atlasSize.width), static_cast<int>(atlasSize.height));

		//return GlyphMap(std::make_shared<OpenGLTexture>(fontPath, texture), glyphMap);
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
}

GlyphLoader::~GlyphLoader()
{
	FT_Done_FreeType(ft);
}
