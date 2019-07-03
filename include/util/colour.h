#ifndef COLOUR_H
#define COLOUR_H

#include <glm/glm.hpp>
#include <string>

namespace Boiler { namespace Colour {

constexpr glm::vec4 fromRGBA(float r, float g, float b, float a)
{
	return glm::vec4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
}

} }
#endif /* COLOUR_H */
