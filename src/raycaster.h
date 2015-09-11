#ifndef RAYCASTER_H
#define RAYCASTER_H

#include <glm/glm.hpp>
#include "rect.h"

class RayCaster
{
    bool clipLine(int d, const Rect &box, const glm::vec2 &v0, const glm::vec2 &v1, float &fLow, float &fHigh);
    bool detectCollisions(const Rect &source, const Rect &dest, float originX, float originY,
                          int numRays, float rayInterval, const glm::vec2 &ray, glm::vec2 &diff);
public:
    bool rayCollides(const glm::vec2 &v0, const glm::vec2 &v1, const Rect &aabb, glm::vec2 &vIntersect);
    bool detectVertical(const Rect &source, const Rect &dest, const glm::vec2 ray, int numRays, bool top, glm::vec2 &diff);
};

#endif /* RAYCASTER_H */
