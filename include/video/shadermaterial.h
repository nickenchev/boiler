#ifndef SHADERMATERIAL_H
#define SHADERMATERIAL_H

#include "core/math.h"

namespace Boiler
{

struct ShaderMaterial
{
	vec4 baseColorFactor;
	uint32_t useBaseTexture;

	ShaderMaterial()
	{
		useBaseTexture = false;
	}
};

}
#endif /* SHADERMATERIAL_H */
