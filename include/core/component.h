#ifndef COMPONENT_H
#define COMPONENT_H

#include "ecstypes.h"

namespace Boiler
{

class Component
{
public:
	virtual const ComponentMask &getMask() const = 0;
};

}

#endif /* COMPONENT_H */
