#ifndef RAYCASTER_H
#define RAYCASTER_H

#include "math.h"
#include "rect.h"

namespace Boiler
{

class RayCaster
{
    bool clipLine(int d, const Rect &box, const vec2 &v0, const vec2 &v1, float &fLow, float &fHigh);
    bool detectCollisions(const Rect &source, const Rect &dest, float originX, float originY,
                          int numRays, vec2 rayInterval, const vec2 &ray, vec2 &diff, bool horizontal);
public:
    bool rayCollides(const vec2 &v0, const vec2 &v1, const Rect &aabb, vec2 &vIntersect, float &lowestFraction);
    bool detectVertical(const Rect &source, const Rect &dest, const vec2 ray, int numRays, bool top, vec2 &diff);
    bool detectHorizontal(const Rect &source, const Rect &dest, const vec2 ray, int numRays, bool left, vec2 &diff);
};

}

#endif /* RAYCASTER_H */
