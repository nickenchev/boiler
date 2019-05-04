#ifndef LINE_H
#define LINE_H

#include <glm/glm.hpp>
#include "rect.h"

namespace Boiler
{

class StandardForm
{
    float a, b, c;
public:
    StandardForm(glm::vec2 p0, glm::vec2 p1);
};

class Line
{
    const glm::vec2 p0, p1;
    float slope;

public:
    Line(glm::vec2 p0, glm::vec2 p1);

    float getSlope() const { return slope; }

    bool intersects(const Line &rect);
};

}

#endif /* LINE_H */
