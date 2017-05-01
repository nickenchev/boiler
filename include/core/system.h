#ifndef SYSTEM_H
#define SYSTEM_H

#include "core/ecstypes.h"
#include "core/componentmapper.h"

class System
{
	ComponentMask systemMask;

public:
	template<typename T>
	System &expects(ComponentMapper &mapper)
	{
		const ComponentMask &mask = mapper.mask<T>();
		systemMask = systemMask & mask;
		return *this;
	}

	virtual void update(const double delta) const = 0;
};

#endif /* SYSTEM_H */
