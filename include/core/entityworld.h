#ifndef ENTITYWORLD_H
#define ENTITYWORLD_H

#include <vector>
#include <memory>
#include "entity.h"
#include "component.h"
#include "logger.h"

namespace Boiler
{

class EntityWorld
{
	Logger logger;
	unsigned long entityCount;
	std::vector<Entity> entities;

public:
    EntityWorld();
	EntityWorld(const EntityWorld &) = delete;
	void operator=(const EntityWorld &) = delete;

    auto &getEntities() const { return entities; }
	Entity createEntity();
    unsigned long count() const { return entityCount; }
	void removeEntity(const Entity &entity);
	bool exists(const Entity &entity) const;
};

}
#endif /* ENTITYWORLD_H */
