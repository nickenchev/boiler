#ifndef COMPONENTMAPPER_H
#define COMPONENTMAPPER_H

#include <unordered_map>
#include <bitset>
#include <iostream>
#include "logger.h"
#include "entity.h"
#include "core/ecstypes.h"

namespace Boiler
{

class ComponentMapper
{
	Logger logger;
	std::unordered_map<EntityId, ComponentMask> componentMap;

public:
	ComponentMapper() : logger{"Component Mapper"} { }
	ComponentMapper(const ComponentMapper &) = delete;
	void operator=(const ComponentMapper &) = delete;

	template<typename T>
	const ComponentMask &add(const Entity &entity)
	{
		// update the entity mask
		ComponentMask &entMask = componentMap[entity.index()];
		entMask = entMask | T::mask;
		return entMask;
	}

	const ComponentMask &add(const Entity &entity, ComponentMask componentMask)
	{
		// update the entity mask
		ComponentMask &entMask = componentMap[entity.index()];
		entMask = entMask | componentMask;
		return entMask;
	}


	const ComponentMask &getMask(const Entity entity)
	{
		return componentMap[entity.index()];
	}

	template<typename T>
	const ComponentMask &remove(const Entity &entity)
	{
		// update the entity mask
		ComponentMask &entMask = componentMap[entity.index()];
		entMask = entMask & ~T::mask;
		return entMask;
	}
};

}
#endif /* COMPONENTMAPPER_H */
