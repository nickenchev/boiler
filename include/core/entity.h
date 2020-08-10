#ifndef ENTITY_H
#define ENTITY_H

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

	void operator=(const Entity &entity)
	{
		this->id = entity.id;
	}
	bool operator==(const Entity &entity) const { return entity.getId() == this->getId(); }
	bool operator!=(const Entity &entity) const { return !(entity == *this); }
	bool operator<(const Entity &entity) const { return getId() < entity.getId(); }
	EntityId getId() const { return id; }
};

}

#endif /* ENTITY_H */
