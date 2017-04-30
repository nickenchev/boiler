#ifndef RAY_H
#define RAY_H

#define NUM_DIMENSIONS 2

struct Rect;

class Ray
{
    glm::vec2 origin, direction, inverseDirection;
    int sign[NUM_DIMENSIONS];

public:
    Ray(const glm::vec2 &p1, const glm::vec2 &p2);

    bool intersects(const Rect &aabb) const;
};

#endif /* RAY_H */
