#ifndef COMPONENTSTORE_H
#define COMPONENTSTORE_H

#include "core/component.h"

typedef std::unordered_map<std::type_index, std::shared_ptr<Component>> ComponentMap;

class ComponentStore
{
	std::unordered_map<EntityId, ComponentMap> entityComponents;

public:
	template<typename T, typename... Args>
	std::shared_ptr<T> store(const Entity &entity, Args&&... args)
	{
		auto component = std::make_shared<T>(std::forward<Args>(args)...);
		entityComponents[entity.getId()][std::type_index(typeid(T))] = component;
		return component;
	}

	template<typename T>
	std::shared_ptr<T> retrieve(const Entity &entity)
	{
		return std::static_pointer_cast<T>(entityComponents[entity.getId()][std::type_index(typeid(T))]);
	}
};


#endif /* COMPONENTSTORE_H */
