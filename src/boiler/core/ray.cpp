#include <glm/glm.hpp>
#include "ray.h"
#include "rect.h"

Ray::Ray(const glm::vec2 &origin, const glm::vec2 &direction) : origin(origin), direction(direction)
{
    inverseDirection = glm::vec2(1 / this->direction.x, 1 / this->direction.y);

    for (int i = 0; i < NUM_DIMENSIONS; ++i)
    {
        sign[i] = (inverseDirection[i] < 0);
    }
}

bool Ray::intersects(const Rect &aabb) const
{
    float tmin, tmax, tymin, tymax;
    glm::vec3 bounds[2] = { aabb.position, glm::vec3(aabb.position.x + aabb.size.getWidth(), aabb.position.y + aabb.size.getHeight(), 0) };

    return true;
}
