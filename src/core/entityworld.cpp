#include <algorithm>
#include "core/entityworld.h"

using namespace Boiler;

EntityWorld::EntityWorld() : logger("Entity World")
{
	entityCount = 0;
}

Entity EntityWorld::createEntity()
{
	Entity entity(++entityCount);
	entities.push_back(entity);
	logger.log("Created entity with ID: {}", entity.getId());
	return entity;
}

void EntityWorld::removeEntity(const Entity &entity)
{
	auto itr = std::find(entities.begin(), entities.end(), entity);
	if (itr != entities.end())
	{
		entities.erase(itr);
		logger.log("Destroyed entity with ID: {}", entity.getId());
	}
}

bool EntityWorld::exists(const Entity &entity) const
{
	bool exists = false;
	auto itr = std::find(entities.begin(), entities.end(), entity);
	if (itr != entities.end())
	{
		exists = true;
	}
	return exists;
}
