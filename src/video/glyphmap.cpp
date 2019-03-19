#include "video/glyphmap.h"
#include "video/opengl.h"

GlyphMap::~GlyphMap()
{
	for (auto itr : map)
	{
		if (itr.second.getTexCoordsVbo())
		{
			auto vbo = itr.second.getTexCoordsVbo();
			glDeleteBuffers(1, &vbo);
		}
	}
}
