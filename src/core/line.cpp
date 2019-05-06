#include "core/line.h"

using namespace Boiler;

StandardForm::StandardForm(glm::vec2 p0, glm::vec2 p1)
{
    a = p1.y - p0.y;
    b = p0.x - p1.x;
    c = a * p0.x + b * p0.y;
}

Line::Line(glm::vec2 p0, glm::vec2 p1) : p0(p0), p1(p1)
{
    // calculate slope: m = (y1 - y2) / (x1 - x2)
    slope = (p0.y - p1.y) / (p0.x - p1.x);
}

bool Line::intersects(const Line &line)
{
    // parallel lines to no intersect
    if (getSlope() == line.getSlope()) return false;

    // generate standard form Ax + Bx = C
    StandardForm standarForm1(p0, p1);
    StandardForm standarForm2(line.p0, line.p1);
    
    bool result = false;
    return result;
}
