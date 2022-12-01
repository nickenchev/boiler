#ifndef AABB_H
#define AABB_H

#include "core/math.h"

namespace Boiler
{
	namespace AABB
	{
		bool intersects(vec3 minA, vec3 maxA, vec3 minB, vec3 maxB);
	}
}

#endif /* AABB_H */
