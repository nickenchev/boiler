#include "core/rect.h"

bool Rect::collides(const Rect &rect) const
{
    cgfloat x1a = position.x;
    cgfloat x2a = position.x + size.width;
    cgfloat y1a = position.y;
    cgfloat y2a = position.y + size.height;

    cgfloat x1b = rect.position.x;
    cgfloat x2b = rect.position.x + rect.size.width;
    cgfloat y1b = rect.position.y;
    cgfloat y2b = rect.position.y + rect.size.height;

    return (x2a >= x1b) && (x1a <= x2b) && (y2a >= y1b) && (y1a <= y2b);
}

bool Rect::collides(const glm::vec2 point) const
{
    //return (point.x >= getMin(DIM_X) && point.x <= getMax(DIM_X) &&
	//point.y >= getMin(DIM_Y) && point.y <= getMax(DIM_Y));
	return false;
}
