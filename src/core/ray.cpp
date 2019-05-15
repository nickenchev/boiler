#include <glm/glm.hpp>
#include "core/ray.h"
#include "core/rect.h"

using namespace Boiler;

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
    //float tmin, tmax, tymin, tymax;
    //glm::vec3 bounds[2] = { aabb.position, glm::vec3(aabb.position.x + aabb.size.width, aabb.position.y + aabb.size.height, 0) };

    return true;
}
