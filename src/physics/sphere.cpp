#include "physics/sphere.h"

float clampAxis(float val, float min, float max)
{
	return std::min(std::max(val, min), max);
}

bool Boiler::Sphere::intersects(vec3 center, float radius, vec3 point)
{
	float length = glm::length(center - point);
	return length < radius;
}

bool Boiler::Sphere::intersects(vec3 center, float radius, vec3 min, vec3 max)
{
	vec3 clamped(clampAxis(center.x, min.x, max.x),
				 clampAxis(center.y, min.y, max.y),
				 clampAxis(center.z, min.z, max.z));
	return intersects(center, radius, clamped);
}
