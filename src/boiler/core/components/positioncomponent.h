#ifndef POSITIONCOMPONENT_H
#define POSITIONCOMPONENT_H

#include "core/component.h"

class PositionComponent : public Component
{
public:
	static int componentId;

    PositionComponent() : Component() { }
};


#endif /* POSITIONCOMPONENT_H */
