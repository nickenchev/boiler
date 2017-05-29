#ifndef SYSTEM_H
#define SYSTEM_H

#include <vector>
#include "core/ecstypes.h"
#include "core/componentmapper.h"

class System
{
	ComponentMask systemMask;
	std::vector<Entity> entities;

public:
	template<typename T>
	System &expects(ComponentMapper &mapper)
	{
		const ComponentMask &mask = mapper.mask<T>();
		systemMask = systemMask | mask;
		return *this;
	}

	void checkEntity(const Entity &entity, const ComponentMask &entityMask)
	{
		if ((entityMask & systemMask) == systemMask)
		{
			// entity is compatible with this system, track it
			entities.push_back(entity);
		}
	}

	virtual void update(const double delta) const = 0;
};

#endif /* SYSTEM_H */
