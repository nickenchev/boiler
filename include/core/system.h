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

	virtual bool checkEntity(const Entity &entity, const ComponentMask &entityMask)
	{
		bool matchingMask = false;
		if ((entityMask & systemMask) == systemMask)
		{
			matchingMask = true;
			// entity is compatible with this system, track it
			entities.push_back(entity);
		}
		return matchingMask;
	}

	virtual void update(ComponentStore &store, const double delta) const = 0;
	const std::vector<Entity> &getEntities() const { return entities; }
};

#endif /* SYSTEM_H */
