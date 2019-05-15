#include "core/rect.h"

using namespace Boiler;

bool Rect::collides(const Rect &rect) const
{
	bool collision = false;
	if (getMaxX() > rect.getMinX() && getMaxY() > rect.getMinY() &&
		getMinX() < rect.getMaxX() && getMinY() < rect.getMaxY())
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
