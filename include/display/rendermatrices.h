#ifndef RENDERMATRICES_H
#define RENDERMATRICES_H

#include "core/common.h"

namespace Boiler
{

struct RenderMatrices
{
	mat4 view;
	mat4 projection;
	mat4 viewProjection;
	mat4 orthographic;
};

}

#endif /* RENDERMATRICES_H */
