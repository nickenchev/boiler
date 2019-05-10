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
    //return (point.x >= getMin(DIM_X) && point.x <= getMax(DIM_X) &&
	//point.y >= getMin(DIM_Y) && point.y <= getMax(DIM_Y));
	return false;
}
