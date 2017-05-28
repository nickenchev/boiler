#ifndef ENTITYCOMPONENTSYSTEM_H
#define ENTITYCOMPONENTSYSTEM_H

#include "core/entityworld.h"
#include "core/componentmapper.h"
#include "core/componentsystems.h"

class Entity;

class EntityComponentSystem
{
	EntityWorld entityWorld;
	ComponentMapper mapper;
	ComponentSystems systems;

public:
	void update(const double delta)
	{
		systems.update(delta);
	}

	Entity newEntity()
	{
		return entityWorld.createEntity();
	}

	template<typename T>
	void addComponent(const Entity &entity)
	{
		auto entMaskPair = mapper.addComponent<T>(entity);
		systems.checkEntity(entMaskPair.first, entMaskPair.second);
	}

	ComponentMapper &getComponentMapper() { return mapper; }
	ComponentSystems &getComponentSystems() { return systems; }
};

#endif /* ENTITYCOMPONENTSYSTEM_H */
