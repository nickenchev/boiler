#ifndef SYSTEM_H
#define SYSTEM_H

#include "core/ecstypes.h"

class System
{
	ComponentMask systemMask;

public:
	virtual void update(const double delta) const = 0;

	System &expects(const ComponentMask &mask)
	{
		systemMask = systemMask & mask;
		return *this;
	}
};

#endif /* SYSTEM_H */
