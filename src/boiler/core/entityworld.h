#ifndef ENTITYWORLD_H
#define ENTITYWORLD_H

#include <vector>
#include <memory>
#include "entity.h"
#include "component.h"
#include "logger.h"

class EntityWorld
{
	Logger logger;
	unsigned long entityCount;
	std::vector<Entity> entities;

public:
    EntityWorld();

	Entity createEntity();
	void removeEntity(const Entity &entity);
	void attachComponent(const Component &component);
};


#endif /* ENTITYWORLD_H */
