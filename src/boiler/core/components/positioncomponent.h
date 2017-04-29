#ifndef POSITIONCOMPONENT_H
#define POSITIONCOMPONENT_H

#include "core/component.h"
#include "core/rect.h"

struct PositionComponent : public Component
{
    Rect rect;
};

#endif /* POSITIONCOMPONENT_H */
