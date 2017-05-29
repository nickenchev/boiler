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

class ComponentMapper
{
	Logger logger;
	unsigned int maskId;
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
	const ComponentMask & addComponent(const Entity &entity)
	{
		// update the entity mask
		const ComponentMask &mask = componentMasks[std::type_index(typeid(T))];
		ComponentMask &entMask = componentMap[entity.getId()];
		entMask = entMask | mask;

		logger.log("Entity Mask: " + entMask.to_string());
		return entMask;
	}

	template<typename T>
	const ComponentMask &mask()
	{
		return componentMasks[std::type_index(typeid(T))];
	}
};

#endif /* COMPONENTMAPPER_H */
