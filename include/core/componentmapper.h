#ifndef COMPONENTMAPPER_H
#define COMPONENTMAPPER_H

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
	std::unordered_map<EntityId, ComponentMask> componentMap;

public:
	ComponentMapper() : logger{"Component Mapper"} { }
	ComponentMapper(const ComponentMapper &) = delete;
	void operator=(const ComponentMapper &) = delete;

	template<typename T>
	const ComponentMask & addComponent(const Entity &entity)
	{
		// update the entity mask
		ComponentMask &entMask = componentMap[entity.getId()];
		entMask = entMask | T::mask;
		return entMask;
	}
};

#endif /* COMPONENTMAPPER_H */
