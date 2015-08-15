#ifndef RAYCASTER_H
#define RAYCASTER_H

#define GLM_COMPILER 0
#include <glm/glm.hpp>
#include "rect.h"

class RayCaster
{
    bool clipLine(int d, const Rect &box, const glm::vec2 &v0, const glm::vec2 &v1, float &fLow, float &fHigh);
public:
    bool rayCollides(const glm::vec2 &v0, const glm::vec2 &v1, const Rect &aabb, glm::vec2 &vIntersect);
};

#endif /* RAYCASTER_H */
