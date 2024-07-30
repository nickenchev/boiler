#ifndef ENTITYCOMPONENTSYSTEM_H
#define ENTITYCOMPONENTSYSTEM_H

#include <string>
#include "core/entity.h"
#include "core/entityworld.h"
#include "core/componentmapper.h"
#include "core/componentsystems.h"
#include "core/componentstore.h"
#include "core/components/parentcomponent.h"
#include "physics/collisioncomponent.h"
#include <queue>

namespace Boiler
{

class EntityComponentSystem
{
	Logger logger;
	EntityWorld entityWorld;
	ComponentMapper mapper;
	ComponentSystems systems;
	ComponentStore componentStore;
	std::queue<std::tuple<Entity, ComponentMask, unsigned int>> removeComponents;
	std::queue<std::tuple<Entity, ComponentMask>> deferredEntityChecks;

	void removeComponent(const Entity &entity, ComponentMask mask, unsigned int storageIndex);

public:
	EntityComponentSystem();
	EntityComponentSystem(const EntityComponentSystem &ecs) = delete;

	EntityComponentSystem &operator=(const EntityComponentSystem &) = delete;

	void update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, SystemStage stage);
	void endFrame();
	Entity newEntity(const std::string &name);
	const std::string &nameOf(Entity entity);
	void setName(Entity entity, const std::string &name);
	void removeEntity(const Entity &entity);
	
	template<typename T>
	T &retrieve(const Entity &entity)
	{
		return getComponentStore().retrieve<T>(entity);
	}

	template<typename T, typename... Args>
	T &createComponent(const Entity &entity, Args&&... args)
	{
		auto entMask = mapper.add<T>(entity);
		T &component = componentStore.store<T>(entity, std::forward<Args>(args)...);
		deferredEntityChecks.push(std::make_tuple(entity, entMask));

		return component;
	}

	template<typename T>
	T &addComponent(const Entity &entity, T &&component)
	{
		auto entMask = mapper.add<T>(entity);
		T &newComponent = componentStore.store<T>(entity, component);
		deferredEntityChecks.push(std::make_tuple(entity, entMask));

		return newComponent;
	}

	template<typename T>
	void removeComponentImmediate(const Entity &entity)
	{
		removeComponent(entity, T::mask, T::storageIndex);
	}

	template<typename T>
	void removeComponent(const Entity &entity)
	{
		removeComponents.push(std::make_tuple(entity, T::mask, T::storageIndex));
	}

	template<typename T, typename... Args>
	T &registerSystem(SystemStage stage, Args&&... args)
	{
		auto &sys = getComponentSystems().registerSystem<T>(stage, std::forward<Args>(args)...);

		for (Entity entity : getEntityWorld().getEntities())
		{
			getComponentSystems().checkEntity(entity, mapper.getMask(entity));
		}

		return sys;
	}

	template<typename T>
	bool hasComponent(const Entity &entity)
	{
		return getComponentStore().hasComponent<T>(entity);
	}

	ComponentStore &getComponentStore() { return componentStore; }
	ComponentSystems &getComponentSystems() { return systems; }
	EntityWorld &getEntityWorld() { return entityWorld; }
};

}
#endif /* ENTITYCOMPONENTSYSTEM_H */
