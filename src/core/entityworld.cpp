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
	for (auto ent = entities.begin(); ent != entities.end(); ent++)
	{
		if (*ent == entity)
		{
			entities.erase(ent);
			logger.log("Destroyed entity with ID: " + std::to_string(entity.getId()));
		}
	}
}
