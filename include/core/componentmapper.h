#ifndef COMPONENTMAPPER_H
#define COMPONENTMAPPER_H

#include <typeindex>
#include <unordered_map>
#include <bitset>
#include <iostream>
#include "logger.h"
#include "entity.h"
#include "core/ecstypes.h"
#include "core/component.h"

typedef std::unordered_map<std::type_index, std::shared_ptr<Component>> ComponentMap;

class ComponentMapper
{
	Logger logger;
	unsigned int maskId;
	std::unordered_map<EntityId, ComponentMap> entityComponents;
	std::unordered_map<EntityId, ComponentMask> componentMap;
	std::unordered_map<std::type_index, ComponentMask> componentMasks;

public:
	ComponentMapper() : logger{"Component Mapper"}, maskId{1} { }
	ComponentMapper(const ComponentMapper &) = delete;
	void operator=(const ComponentMapper &) = delete;

	template<typename T>
	void registerComponent()
	{
		const ComponentMask mask = ComponentMask{maskId};
		componentMasks[std::type_index(typeid(T))] = mask;
		maskId *= 2;

		logger.log("Registered component with mask: " + mask.to_string());
	}

	template<typename T>
	std::pair<const Entity &, const ComponentMask &> addComponent(const Entity &entity)
	{
		// update the component mask
		const ComponentMask &mask = componentMasks[std::type_index(typeid(T))];
		ComponentMask &entMask = componentMap[entity.getId()];
		entMask = entMask | mask;
		logger.log("Entity Mask: " + entMask.to_string());

		// add the component objcet for this entity
		entityComponents[entity.getId()][std::type_index(typeid(T))] = std::make_shared<T>();

		return std::make_pair(entity, entMask);
	}

	template<typename T>
	std::shared_ptr<T> getComponent(const Entity &entity)
	{
		return entityComponents[entity.getId()][std::type_index(typeid(T))];
	}

	template<typename T>
	ComponentMask mask()
	{
		return componentMasks[std::type_index(typeid(T))];
	}
};

#endif /* COMPONENTMAPPER_H */
