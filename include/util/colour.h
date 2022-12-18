#ifndef COLOUR_H
#define COLOUR_H

#include "core/math.h"

namespace Boiler { namespace Colour {

constexpr vec4 fromRGBA(float r, float g, float b, float a)
{
	return vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

} }
#endif /* COLOUR_H */
