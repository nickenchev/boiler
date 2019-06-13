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
	logger.log("Created entity with ID: " + std::to_string(entity.getId()));
	return entity;
}

void EntityWorld::removeEntity(const Entity &entity)
{
	auto itr = std::find(entities.begin(), entities.end(), entity);
	if (itr != entities.end())
	{
		Entity ent = *itr;
		entities.erase(itr);
		logger.log("Destroyed entity with ID: " + std::to_string(ent.getId()));
	}
}
