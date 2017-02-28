#include "entityworld.h"

EntityWorld::EntityWorld()
{
	entityCount = 0;
}

Entity EntityWorld::createEntity()
{
	Entity entity(++entityCount);
	entities.push_back(entity);
	return entity;
}
