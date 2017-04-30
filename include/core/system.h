#ifndef SYSTEM_H
#define SYSTEM_H

#include "core/ecstypes.h"
#include "core/componentmapper.h"

class System
{
	const ComponentMapper &mapper;
	ComponentMask systemMask;

public:
	System(const ComponentMapper &mapper) : mapper(mapper) { }
	System &expects(const ComponentMask &mask)
	{
		systemMask = systemMask & mask;
		return *this;
	}

	virtual void update(const double delta) const = 0;
};

#endif /* SYSTEM_H */
