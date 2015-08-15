#include <algorithm>
#include "raycaster.h"

bool RayCaster::clipLine(int d, const Rect &box, const glm::vec2 &v0, const glm::vec2 &v1, float &fLow, float &fHigh)
{
    bool isIntersection = true;

    // get the fraction of the line (v0->v1) that intersects the box
    float boxDimLow = box.position[d];
    float boxDimHigh = box.position[d] + ((d == 0) ? box.size.width : box.size.height);
    float fDimLow = (boxDimLow - v0[d]) / (v1[d] - v0[d]);
    float fDimHigh = (boxDimHigh - v0[d]) / (v1[d] - v0[d]);

    // swap if needed, varies depending on direction of ray
    if (fDimLow > fDimHigh)
    {
        std::swap(fDimLow, fDimHigh);
    }

    // make sure we're not completely missing
    if (fDimHigh < fLow)
    {
        isIntersection = false;
    }
    else if (fDimLow > fHigh)
    {
        isIntersection = false;
    }
    else
    {
        fLow = std::max(fDimLow, fLow);
        fHigh = std::min(fDimHigh, fHigh);
    }

    if (fLow > fHigh)
    {
        isIntersection = false;
    }

    return isIntersection;
}

bool RayCaster::rayCollides(const glm::vec2 &v0, const glm::vec2 &v1, const Rect &aabb, glm::vec2 &vIntersect)
{
    // ray cast and see if the player collides with this
    float fLow = 0;
    float fHigh = 1;

    // clip x (0) and y(0) dimensions to get the line segment that collides
    bool isIntersection = false;
    if (clipLine(0, aabb, v0, v1, fLow, fHigh))
    {
        if (clipLine(1, aabb, v0, v1, fLow, fHigh))
        {
            // get the full vector (b) and the vector the represents the first intersection point on the box
            glm::vec2 b = v1 - v0;
            vIntersect = v0 + b * fLow; // fLow is the fraction that represents the initial intersection

            isIntersection = true;
        }
    }
    return isIntersection;
}
