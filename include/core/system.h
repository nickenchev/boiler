#ifndef SYSTEM_H
#define SYSTEM_H

#include <SDL.h>
#include <vector>
#include <string>
#include "core/ecstypes.h"
#include "core/componentmapper.h"
#include "core/logger.h"

namespace Boiler
{

class ComponentStore;

class System
{
	ComponentMask systemMask;
	std::vector<Entity> entities;
	const std::string name;

protected:
	const Logger logger;

public:
	System(std::string name) : name(name), logger(name) { }
	virtual ~System() { }

	template<typename T>
	System &expects()
	{
		systemMask = systemMask | T::mask;
		logger.log("System mask: " + systemMask.to_string());
		return *this;
	}

	virtual bool checkEntity(const Entity &entity, const ComponentMask &entityMask)
	{
		bool matchingMask = false;
		if ((entityMask & systemMask) == systemMask)
		{
			// ensure entity isn't already in here
			if (std::find(entities.begin(), entities.end(), entity) == entities.end())
			{
				matchingMask = true;
				// entity is compatible with this system, track it
				entities.push_back(entity);
				logger.log("Entity #" + std::to_string(entity.getId()) + " with signature: " + entityMask.to_string() + ".");
			}
		}
		return matchingMask;
	}

	virtual void update(ComponentStore &store, const double delta) = 0;
	const std::vector<Entity> &getEntities() const { return entities; }
	const std::string &getName() const { return name; }
};

}
#endif /* SYSTEM_H */
