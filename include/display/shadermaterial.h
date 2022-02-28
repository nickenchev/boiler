#ifndef SHADERMATERIAL_H
#define SHADERMATERIAL_H

#include "core/math.h"

namespace Boiler
{

struct ShaderMaterial
{
	alignas(16) vec4 baseColorFactor;

	ShaderMaterial()
	{
	}
};

}
#endif /* SHADERMATERIAL_H */
