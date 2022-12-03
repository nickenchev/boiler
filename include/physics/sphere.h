#ifndef SPHERE_H
#define SPHERE_H

#include "core/math.h"

namespace Boiler
{
	namespace Sphere {
		bool intersects(vec3 center, float radius, vec3 point);
		bool intersects(vec3 center, float radius, vec3 min, vec3 max);
	}
}

#endif /* SPHERE_H */
