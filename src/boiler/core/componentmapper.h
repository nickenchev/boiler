#ifndef COMPONENTMAPPER_H
#define COMPONENTMAPPER_H

#include <typeindex>
#include <unordered_map>
#include <bitset>
#include <iostream>
#include "logger.h"
#include "entity.h"

#define MAX_COMPONENTS 64

class ComponentMapper
{
	Logger logger;
	unsigned int maskId;
	std::unordered_map<EntityId, std::bitset<MAX_COMPONENTS>> componentMap;
	std::unordered_map<std::type_index, std::bitset<MAX_COMPONENTS>> componentMasks;

public:
	ComponentMapper() : logger{"Component Mapper"}, maskId{1} { }
	ComponentMapper(const ComponentMapper &) = delete;
	void operator=(const ComponentMapper &) = delete;

	template<typename T>
	void registerComponent()
	{
		const std::bitset<MAX_COMPONENTS> mask = std::bitset<MAX_COMPONENTS>{maskId};
		componentMasks[std::type_index(typeid(T))] = mask;
		logger.log("Registered component with mask: " + mask.to_string());
		maskId *= 2;
	}

	template<typename T>
	void addComponent(const Entity &entity)
	{
		const std::bitset<MAX_COMPONENTS> &mask = componentMasks[std::type_index(typeid(T))];
		std::bitset<MAX_COMPONENTS> &entMask = componentMap[entity.getId()];
		entMask = entMask | mask;

		logger.log("Entity Mask: " + entMask.to_string());
	}
};

#endif /* COMPONENTMAPPER_H */
