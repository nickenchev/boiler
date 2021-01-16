#ifndef SHADERMATERIAL_H
#define SHADERMATERIAL_H

#include "core/math.h"

namespace Boiler
{

struct ShaderMaterial
{
	alignas(16) vec4 baseColorFactor;
	uint32_t useBaseTexture; // TODO: Why align this way?

	ShaderMaterial()
	{
		useBaseTexture = false;
	}
};

}
#endif /* SHADERMATERIAL_H */
