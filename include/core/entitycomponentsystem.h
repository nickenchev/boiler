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

namespace Boiler
{

class EntityComponentSystem
{
	Logger logger;
	EntityWorld entityWorld;
	ComponentMapper mapper;
	ComponentSystems systems;
	ComponentStore componentStore;
	std::vector<std::tuple<Entity, ComponentMask, unsigned int>> removeComponents;

	void removeComponent(const Entity &entity, ComponentMask mask, unsigned int storageIndex);

public:
	EntityComponentSystem();
	EntityComponentSystem(const EntityComponentSystem &ecs) = delete;

	EntityComponentSystem &operator=(const EntityComponentSystem &) = delete;

	void update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, SystemStage stage);
	void endFrame();
	Entity newEntity(const std::string &name);
	const std::string &nameOf(Entity entity);
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
		systems.checkEntity(entity, entMask);

		return component;
	}

	template<typename T>
	void removeComponentImmediate(const Entity &entity)
	{
		removeComponent(entity, T::mask, T::storageIndex);
	}

	template<typename T>
	void removeComponent(const Entity &entity)
	{
		removeComponents.push_back(std::make_tuple(entity, T::mask, T::storageIndex));
	}

	ComponentStore &getComponentStore() { return componentStore; }
	ComponentSystems &getComponentSystems() { return systems; }
	const EntityWorld &getEntityWorld() const { return entityWorld; }
};

}
#endif /* ENTITYCOMPONENTSYSTEM_H */
