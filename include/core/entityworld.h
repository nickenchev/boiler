#ifndef ENTITYWORLD_H
#define ENTITYWORLD_H

#include <vector>
#include <array>
#include <memory>
#include <string>
#include "entity.h"
#include "logger.h"

namespace Boiler
{

class EntityWorld
{
	Logger logger;
	unsigned long entityCount;
	std::vector<Entity> entities;
	std::array<std::string, MAX_ENTITIES> names;

public:
    EntityWorld();
	EntityWorld(const EntityWorld &) = delete;
	void operator=(const EntityWorld &) = delete;

    auto &getEntities() const { return entities; }
	Entity createEntity(const std::string &name);
    unsigned long count() const { return entityCount; }
	void removeEntity(const Entity &entity);
	bool exists(const Entity &entity) const;

	const std::string &getName(Entity entity) const;
};

}
#endif /* ENTITYWORLD_H */
