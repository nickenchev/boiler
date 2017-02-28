#ifndef POSITIONCOMPONENT_H
#define POSITIONCOMPONENT_H

#include "core/component.h"

class PositionComponent : public Component
{
public:
    PositionComponent() : Component(COMPONENT_POSITION) { }
};


#endif /* POSITIONCOMPONENT_H */
