#include "rect.h"

bool Rect::collides(const Rect &rect)
{
    int x1a = position.x;
    int x2a = position.x + size.getWidth();
    int y1a = position.y;
    int y2a = position.y + size.getHeight();

    int x1b = rect.position.x;
    int x2b = rect.position.x + rect.size.getWidth();
    int y1b = rect.position.y;
    int y2b = rect.position.y + rect.size.getHeight();

    return (x2a >= x1b) && (x1a <= x2b) && (y2a >= y1b) && (y1a <= y2b);
}

