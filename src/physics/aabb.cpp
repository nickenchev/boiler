#include "physics/aabb.h"

bool Boiler::AABB::intersects(vec3 minA, vec3 maxA, vec3 minB, vec3 maxB)
{
	return maxA.x > minB.x && minA.x < maxB.x && maxA.y > minB.y && minA.y < maxB.y && maxA.z > minB.z && minA.z < maxB.z;
}
