#ifndef COMPONENTMAPPER_H
#define COMPONENTMAPPER_H

#include <unordered_map>
#include <bitset>
#include "entity.h"

#define MAX_COMPONENTS 64

class ComponentMapper
{
	std::unordered_map<EntityId, std::bitset<MAX_COMPONENTS>> componentMap;

public:
	ComponentMapper() { }
	ComponentMapper(const ComponentMapper &) = delete;
	void operator=(const ComponentMapper &) = delete;

	template <typename T>
	void registerComponent() { T::componentId = 1; }
};


#endif /* COMPONENTMAPPER_H */
