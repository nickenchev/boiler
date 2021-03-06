#ifndef ENTITYCOMPONENTSYSTEM_H
#define ENTITYCOMPONENTSYSTEM_H

#include <string>
#include "core/common.h"
#include "core/entityworld.h"
#include "core/componentmapper.h"
#include "core/componentsystems.h"
#include "core/componentstore.h"
#include "core/components/parentcomponent.h"

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
	EntityComponentSystem(const EntityComponentSystem &ecs) = delete;
	EntityComponentSystem &operator=(const EntityComponentSystem &) = delete;

	void update(const Time deltaTime, const Time globalTime)
	{
		systems.update(componentStore, deltaTime, globalTime);
	}

	Entity newEntity()
	{
		return entityWorld.createEntity();
	}

	Entity duplicate(const Entity entity)
	{
		Entity newEntity = entityWorld.createEntity();
		auto &components = componentStore.copyComponents(entity, newEntity);

		// ensure new components are mapped and systems are aware
		for (const auto &c : components)
		{
			if (c != nullptr) mapper.add(newEntity, c->getMask());
		}
		const ComponentMask &entityMask = mapper.getMask(newEntity);
		systems.checkEntity(newEntity, entityMask);

		logger.log("Duplicated entity {} -> {} with mask: ", entity.getId(), newEntity.getId(), mapper.getMask(newEntity).to_string());

		return newEntity;
	}

	void removeEntity(const Entity &entity)
	{
		logger.log("Deleting entity #: " + std::to_string(entity.getId()));

		/*
		// find children and remove
		auto children = componentStore.find<ParentComponent>();
		for (auto child : children)
		{
			ParentComponent &parent = componentStore.retrieve<ParentComponent>(child);
			if (parent.entity == entity)
			{
				removeEntity(child);
			}
		}

		// remove entity itself
		systems.removeEntity(entity);
		componentStore.removeAll(entity);
		entityWorld.removeEntity(entity);
		*/
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
	const EntityWorld &getEntityWorld() const { return entityWorld; }
};

}
#endif /* ENTITYCOMPONENTSYSTEM_H */
