#ifndef LINE_H
#define LINE_H

#include <glm/glm.hpp>
#include "rect.h"

class Line
{
    const glm::vec2 p0, p1;
    float slope;

public:
    Line(glm::vec2 p0, glm::vec2 p1);

    inline bool intersects(const Rect &rect)
    {
        return true;
    }
};


#endif /* LINE_H */
