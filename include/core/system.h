#ifndef SYSTEM_H
#define SYSTEM_H

#include <type_traits>
#include <SDL.h>
#include <vector>
#include <algorithm>
#include <string>
#include "core/common.h"
#include "core/frameinfo.h"
#include "core/entity.h"
#include "core/componentmapper.h"
#include "core/logger.h"
#include "assets/assetset.h"
#include "core/component.h"

namespace Boiler
{

class Renderer;
class EntityComponentSystem;

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
		static_assert(std::is_base_of<Component, T>(), "Component class not specified.");
		systemMask = systemMask | T::mask;
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
				logger.log("Tracking entity {}", entity.getId());
			}
		}
		else if (entityItr != entities.end())
		{
			// entity no longer matches mask, but is in this system, remove it
			logger.log("No longer tracking entity {}", entity.getId());
			entities.erase(entityItr);
		}

		return matchingMask;
	}

    virtual void update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, EntityComponentSystem &ecs) = 0;
	const std::vector<Entity> &getEntities() const { return entities; }
	const std::string &getName() const { return name; }
	void removeEntity(const Entity &entity)
	{
		auto itr = std::find(entities.begin(), entities.end(), entity);
		if (itr != entities.end())
		{
			entities.erase(itr);
		}
	}
};

}
#endif /* SYSTEM_H */
