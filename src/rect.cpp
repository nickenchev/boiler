#include "rect.h"

bool Rect::intersects(const Rect &rect)
{
    int x1a = position.x;
    int x2a = position.x + size.width;
    int y1a = position.y;
    int y2a = position.y + size.height;

    int x1b = rect.position.x;
    int x2b = rect.position.x + rect.size.width;
    int y1b = rect.position.y;
    int y2b = rect.position.y + rect.size.height;

    return (x2a >= x1b) && (x1a <= x2b) && (y2a >= y1b) && (y1a <= y2b);
}

glm::vec2 Rect::getCentre() const
{
    float x = position.x + size.width / 2;
    float y = position.y + size.height / 2;

    return glm::vec2(x, y);
}
