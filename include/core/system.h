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
		// check if entity exists in this system
		auto entityItr = std::find(entities.begin(), entities.end(), entity);

		if ((entityMask & systemMask) == systemMask)
		{
			// ensure entity isn't already in here
			if (entityItr == entities.end())
			{
				// entity is compatible with this system, track it
				matchingMask = true;
				entities.push_back(entity);
				logger.log("Adding entity #" + std::to_string(entity.getId()) + " with signature: " + entityMask.to_string());
			}
		}
		else if (entityItr != entities.end())
		{
			// entity no longer matches mask, but is in this system, remove it
			logger.log("Removing entity #" + std::to_string(entity.getId()) + " with signature: " + entityMask.to_string());
			entities.erase(entityItr);
		}

		return matchingMask;
	}

	virtual void update(ComponentStore &store, const double delta) = 0;
	const std::vector<Entity> &getEntities() const { return entities; }
	const std::string &getName() const { return name; }
};

}
#endif /* SYSTEM_H */
