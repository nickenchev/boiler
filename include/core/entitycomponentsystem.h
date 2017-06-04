#ifndef ENTITYCOMPONENTSYSTEM_H
#define ENTITYCOMPONENTSYSTEM_H

#include "core/entityworld.h"
#include "core/componentmapper.h"
#include "core/componentsystems.h"
#include "core/componentstore.h"

class Entity;

class EntityComponentSystem
{
	EntityWorld entityWorld;
	ComponentMapper mapper;
	ComponentSystems systems;
	ComponentStore componentStore;

public:
	void update(const double delta)
	{
		systems.update(componentStore, delta);
	}

	Entity newEntity()
	{
		return entityWorld.createEntity();
	}

	template<typename T>
	void addComponent(const Entity &entity)
	{
		auto entMask = mapper.addComponent<T>(entity);
		componentStore.store<T>(entity);
		systems.checkEntity(entity, entMask);
	}

	ComponentMapper &getComponentMapper() { return mapper; }
	ComponentSystems &getComponentSystems() { return systems; }
};

#endif /* ENTITYCOMPONENTSYSTEM_H */
