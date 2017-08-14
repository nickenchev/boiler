#ifndef COMPONENTSTORE_H
#define COMPONENTSTORE_H

#include "core/component.h"

typedef std::unordered_map<ComponentMask, std::shared_ptr<Component>> ComponentMap;

class ComponentStore
{
	std::unordered_map<EntityId, ComponentMap> entityComponents;

public:
	template<typename T, typename... Args>
	std::shared_ptr<T> store(const Entity &entity, Args&&... args)
	{
		// construct and store the new compnent for this entity
		auto component = std::make_shared<T>(std::forward<Args>(args)...);
		entityComponents[entity.getId()][T::mask] = component;

		return component;
	}

	template<typename T>
	T &retrieve(const Entity &entity)
	{
		return *std::static_pointer_cast<T>(entityComponents[entity.getId()][T::mask]);
	}
};


#endif /* COMPONENTSTORE_H */
