#include "line.h"

Line::Line(glm::vec2 p0, glm::vec2 p1) : p0(p0), p1(p1)
{
    // calculate slope: m = (y1 - y2) / (x1 - x2)
    slope = (p0.y - p1.y) / (p0.x - p1.x);
}
