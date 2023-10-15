#ifndef ENTITY_H
#define ENTITY_H

#include <stddef.h>

constexpr unsigned int MAX_ENTITIES = 5000;

namespace Boiler
{

using EntityId = unsigned long;

class Entity
{
	EntityId id;
    
public:
	Entity() { id = 0; }
	Entity(EntityId id) : id{id} { }
	Entity(const Entity &entity) : Entity(entity.getId()) { }

	static constexpr EntityId NO_ENTITY = 0;

	void operator=(const Entity &entity)
	{
		this->id = entity.id;
	}
	bool operator==(const Entity &entity) const { return entity.getId() == this->getId(); }
	bool operator!=(const Entity &entity) const { return !(entity == *this); }
	bool operator!=(const EntityId &entityId) const { return !(entityId == getId()); }
	bool operator<(const Entity &entity) const { return getId() < entity.getId(); }
	operator bool() const { return getId() != 0; }
	EntityId getId() const { return id; }

	inline int index() const
	{
		return getId() - 1;
	}

};

}

#endif /* ENTITY_H */
