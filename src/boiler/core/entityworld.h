#ifndef ENTITYWORLD_H
#define ENTITYWORLD_H

#include <vector>
#include <memory>
#include "entity.h"
#include "component.h"

class EntityWorld
{
	unsigned long entityCount;
	std::vector<Entity> entities;

public:
    EntityWorld();

	Entity createEntity();
	void attachComponent(const Component &component);
};


#endif /* ENTITYWORLD_H */
