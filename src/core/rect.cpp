#include "core/rect.h"

using namespace Boiler;

bool Rect::collides(const Rect &rect) const
{
	bool collision = false;

	cgfloat epsilon = 0.001f;
	cgfloat xDiff = getMaxX() - rect.getMinX();
	cgfloat yDiff = getMaxY() - rect.getMinY();
	cgfloat xDiffMin = rect.getMaxX() - getMinX();
	cgfloat yDiffMin = rect.getMaxY() - getMinY();

	if (xDiff > epsilon && yDiff > epsilon &&
		xDiffMin > epsilon && yDiffMin > epsilon)
	{
		collision = true;
	}

	return collision;
}

bool Rect::collides(const glm::vec2 point) const
{
	// TODO: Needs testing
    return (point.x >= getMinX() && point.x <= getMaxX() &&
	point.y >= getMinY() && point.y <= getMaxY());
}
