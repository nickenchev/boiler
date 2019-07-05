#include "core/rect.h"

using namespace Boiler;

bool Rect::collides(const Rect &rect, const vec3 &epsilon) const
{
	bool collision = false;

	if (getMaxX() > rect.getMinX() + epsilon.x &&
		getMinX() < rect.getMaxX() - epsilon.x &&
		getMaxY() > rect.getMinY() + epsilon.y &&
		getMinY() < rect.getMaxY() - epsilon.y)
	{
		collision = true;
	}
	/*
	cgfloat xDiff = getMaxX() - rect.getMinX();
	cgfloat yDiff = getMaxY() - rect.getMinY();
	cgfloat xDiffMin = rect.getMaxX() - getMinX();
	cgfloat yDiffMin = rect.getMaxY() - getMinY();

	// distance between x/y edges must be less than the
	// epsilon value to register a collision
	if ((xDiff > epsilon.x && xDiffMin > epsilon.x) &&
		(yDiff > epsilon.y && yDiffMin > epsilon.y))
	{
		collision = true;
	}
	*/

	return collision;
}

bool Rect::collides(const vec2 point) const
{
	// TODO: Needs testing
    return (point.x >= getMinX() && point.x <= getMaxX() &&
	point.y >= getMinY() && point.y <= getMaxY());
}
