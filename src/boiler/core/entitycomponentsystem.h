#ifndef ENTITYCOMPONENTSYSTEM_H
#define ENTITYCOMPONENTSYSTEM_H

#include "core/entityworld.h"
#include "core/componentmapper.h"
#include "core/componentsystems.h"

struct EntityComponentSystem
{
	EntityWorld entityWorld;
	ComponentMapper mapper;
	ComponentSystems systems;

	void update(const double delta)
	{
		systems.update(delta);
	}
};

#endif /* ENTITYCOMPONENTSYSTEM_H */
