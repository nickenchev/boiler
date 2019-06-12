#ifndef PARENTCOMPONENT_H
#define PARENTCOMPONENT_H

#include "core/componenttype.h"
#include "core/entity.h"

struct ParentComponent : Boiler::ComponentType<ParentComponent>
{
	Boiler::Entity entity;

	ParentComponent(const Boiler::Entity parentEntity)
	{
		this->entity = parentEntity;
	}
};

#endif /* PARENTCOMPONENT_H */
