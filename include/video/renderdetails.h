#ifndef RENDERDETAILS_H
#define RENDERDETAILS_H

#include "video/shaderprogram.h"
#include "core/math.h"

namespace Boiler
{

struct RenderDetails
{
	const ShaderProgram *shaderProgram;
	int mvpUniform;
	int colorUniform;
    mat4 viewProjection;
    mat4 camViewProjection;

    RenderDetails()
    {
        mvpUniform = -1;
        colorUniform = -1;
    }
};

}

#endif /* RENDERDETAILS_H */
