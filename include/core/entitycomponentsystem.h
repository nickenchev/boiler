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

	template<typename T, typename... Args>
	std::shared_ptr<T> createComponent(const Entity &entity, Args&&... args)
	{
		auto entMask = mapper.addComponent<T>(entity);
		auto component = componentStore.store<T>(entity, std::forward<Args>(args)...);
		systems.checkEntity(entity, entMask);

		return component;
	}

	ComponentStore &getComponentStore() { return componentStore; }
	ComponentMapper &getComponentMapper() { return mapper; }
	ComponentSystems &getComponentSystems() { return systems; }
};

#endif /* ENTITYCOMPONENTSYSTEM_H */
