#ifndef COMPONENT_H
#define COMPONENT_H

#include "ecstypes.h"

namespace Boiler
{

class Component
{
public:
	virtual ~Component() {}
	virtual const ComponentMask &getMask() const = 0;
};

}

#endif /* COMPONENT_H */
