#ifndef SYSTEM_H
#define SYSTEM_H

#include <vector>
#include "core/ecstypes.h"
#include "core/componentmapper.h"

class ComponentStore;

class System
{
	ComponentMask systemMask;
	std::vector<Entity> entities;

public:
	virtual ~System() { }

	template<typename T>
	System &expects()
	{
		systemMask = systemMask | T::mask;
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

	virtual void update(ComponentStore &store, const double delta) const = 0;
	const std::vector<Entity> &getEntities() const { return entities; }
};

#endif /* SYSTEM_H */
