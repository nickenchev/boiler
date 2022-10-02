#ifndef CHILDRENCOMPONENT_H
#define CHILDRENCOMPONENT_H

#include <array>
#include <assert.h>
#include "core/componenttype.h"
#include "core/entity.h"

namespace Boiler {

class ChildrenComponent : Boiler::ComponentType<ChildrenComponent>
{
	constexpr static unsigned int maxChildren = 64;
	unsigned int insertIndex;
public:
	std::array<Boiler::Entity, maxChildren> childEntities;

	void addChild(Entity entity)
	{
		assert(insertIndex < maxChildren);
		childEntities[insertIndex++] = entity;
	}
};

}

#endif /* CHILDRENCOMPONENT_H */
