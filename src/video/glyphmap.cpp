#include "video/glyphmap.h"
#include "video/opengl.h"

using namespace Boiler;

GlyphMap::~GlyphMap()
{
	for (auto itr : map)
	{
		const Glyph &glyph = itr.second;
		if (glyph.getTexCoordsVbo())
		{
			GLuint vbo = glyph.getTexCoordsVbo();
			glDeleteBuffers(1, &vbo);
		}
	}
}
