#ifndef OPENGLTEXTUREINFO_H
#define OPENGLTEXTUREINFO_H

#include "video/opengl.h"
#include "video/textureinfo.h"

namespace Boiler
{

class OpenGLTextureInfo : public TextureInfo
{
	GLuint texCoordsVbo;

public:
	OpenGLTextureInfo(GLuint texCoordsVbo)
	{
		this->texCoordsVbo = texCoordsVbo;
	}

	~OpenGLTextureInfo()
	{
        if (texCoordsVbo)
        {
            glDeleteBuffers(1, &texCoordsVbo);
			texCoordsVbo = 0;
        }
	}

	GLuint getTexCoordsVbo() const { return texCoordsVbo; }
};

}


#endif /* OPENGLTEXTUREINFO_H */
