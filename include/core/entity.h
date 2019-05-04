#ifndef ENTITY_H
#define ENTITY_H

namespace Boiler
{

typedef unsigned long EntityId;

class Entity
{
	EntityId id;
    
public:
	Entity() { id = 0; }
	Entity(EntityId id) : id{id} { }
	Entity(const Entity &entity) : Entity(entity.getId()) { }

	bool operator==(const Entity &entity) const { return entity.getId() == this->getId(); }
	EntityId getId() const { return id; }
};

}

#endif /* ENTITY_H */
