#ifndef SYSTEM_H
#define SYSTEM_H

#include <SDL2/SDL.h>
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

protected:
	const Logger logger;

public:
	System(std::string name) : name(name), logger(name) { }
	virtual ~System() { }

	const std::string name;

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
			matchingMask = true;
			// entity is compatible with this system, track it
			entities.push_back(entity);
		}
		return matchingMask;
	}

	virtual void update(ComponentStore &store, const double delta) const = 0;
	virtual void processEvent(const SDL_Event &event) const { }
	const std::vector<Entity> &getEntities() const { return entities; }
};

}
#endif /* SYSTEM_H */
