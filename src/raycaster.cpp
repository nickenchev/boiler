#include <iostream>
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

bool RayCaster::detectCollisions(const Rect &source, const Rect &dest, float originX, float originY,
                                 int numRays, float rayInterval, const glm::vec2 &ray, glm::vec2 &diff)
{
    bool collision = false;
    float maxOffset = source.getMaxX();
#ifdef DEBUG
    std::cout << "-----------------------------------" << std::endl;
    std::cout << "X: " << source.getMinX() << "  " << source.getMaxX() << std::endl;
#endif
    for (int r = 0; r < numRays; ++r)
    {
        float rayOffset = r * rayInterval;
        float xOffset = originX + rayOffset;
        xOffset = (xOffset > maxOffset) ? maxOffset : xOffset;

        glm::vec2 v0(xOffset, originY);
        glm::vec2 v1 = v0 + ray;

#ifdef DEBUG
        std::cout << "(" << v0.x << "," << v0.y << ") --> ";
        std::cout << "(" << v1.x << "," << v1.y << ")" << std::endl;
#endif
        
        glm::vec2 vIntersect;
        if (rayCollides(v0, v1, dest, vIntersect))
        {
#ifdef DEBUG
            std::cout << "  --- "<< dest.getMinX() << "," << dest.getMaxX() << std::endl;
#endif
            // diff is the line between v0 and point of intersection
            glm::vec2 diffTemp = vIntersect - v0;
            float intersectLength = diffTemp.length();
            if (intersectLength >= 0)
            {
                diff = diffTemp;
                collision = true;
            }
        }
    }

    return collision;
}

bool RayCaster::detectVertical(const Rect &source, const Rect &dest, const glm::vec2 ray, int numRays, bool top, glm::vec2 &diff)
{
    float rayInterval = source.size.width / (numRays - 1);
    float originY = top ? source.getMinY() : source.getMaxY();
    return detectCollisions(source, dest, source.getMinX(), originY, numRays, rayInterval, glm::vec2(0, ray.y), diff);
}
