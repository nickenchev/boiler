#ifndef VELOCITYCOMPONENT_H
#define VELOCITYCOMPONENT_H

#include "core/componenttype.h"

struct VelocityComponent : public ComponentType<VelocityComponent>
{
    glm::vec3 velocity;
};

#endif /* VELOCITYCOMPONENT_H */
