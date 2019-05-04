#ifndef RENDERDETAILS_H
#define RENDERDETAILS_H

#include <glm/glm.hpp>
#include "video/shaderprogram.h"

namespace Boiler
{

struct RenderDetails
{
	const ShaderProgram *shaderProgram;
	int mvpUniform;
	int colorUniform;
    glm::mat4 viewProjection;
    glm::mat4 camViewProjection;

    RenderDetails()
    {
        mvpUniform = -1;
        colorUniform = -1;
    }
};

}

#endif /* RENDERDETAILS_H */
