#ifndef ENTITYCOMPONENTSYSTEM_H
#define ENTITYCOMPONENTSYSTEM_H

#include <string>
#include "core/entityworld.h"
#include "core/componentmapper.h"
#include "core/componentsystems.h"
#include "core/componentstore.h"

namespace Boiler
{

class Entity;

class EntityComponentSystem
{
	Logger logger;
	EntityWorld entityWorld;
	ComponentMapper mapper;
	ComponentSystems systems;
	ComponentStore componentStore;

public:
	EntityComponentSystem() : logger("ECS") { }

	void update(const double delta)
	{
		systems.update(componentStore, delta);
	}

	Entity newEntity()
	{
		return entityWorld.createEntity();
	}

	void removeEntity(const Entity &entity)
	{
		systems.removeEntity(entity);
		componentStore.removeAll(entity);
		entityWorld.removeEntity(entity);
	}

	template<typename T, typename... Args>
	std::shared_ptr<T> createComponent(const Entity &entity, Args&&... args)
	{
		auto entMask = mapper.add<T>(entity);
		auto component = componentStore.store<T>(entity, std::forward<Args>(args)...);
		systems.checkEntity(entity, entMask);

		return component;
	}

	template<typename T>
	void removeComponent(const Entity &entity)
	{
		auto entMask = mapper.remove<T>(entity);
		componentStore.remove<T>(entity);
		systems.checkEntity(entity, entMask);
	}

	ComponentStore &getComponentStore() { return componentStore; }
	ComponentSystems &getComponentSystems() { return systems; }
};

}
#endif /* ENTITYCOMPONENTSYSTEM_H */
