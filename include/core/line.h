#ifndef LINE_H
#define LINE_H

#include "core/math.h"
#include "rect.h"

namespace Boiler
{

class StandardForm
{
    float a, b, c;
public:
    StandardForm(vec2 p0, vec2 p1);
};

class Line
{
    const vec2 p0, p1;
    float slope;

public:
    Line(vec2 p0, vec2 p1);

    float getSlope() const { return slope; }

    bool intersects(const Line &rect);
};

}

#endif /* LINE_H */
