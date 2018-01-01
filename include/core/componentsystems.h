#ifndef COMPONENTSYSTEMS_H
#define COMPONENTSYSTEMS_H

#include <vector>
#include <memory>
#include "core/ecstypes.h"
#include "core/system.h"
#include "core/logger.h"
#include "core/componentmapper.h"
#include "core/componentstore.h"
#include "core/messagedispatch.h"

class ComponentSystems
{
	Logger logger;
	std::vector<std::unique_ptr<System>> systems;

public:
    ComponentSystems() : logger{"ComponentSystems"}  { }
    virtual ~ComponentSystems() { }

	void update(ComponentStore &store, const double delta)
	{
		for (auto &system : systems)
		{
			system->update(store, delta);
		}
	}

	template<class T>
	System &registerSystem()
	{
		// TODO: Check if any of the existing entities fit into the newly registered system
		auto system = std::make_unique<T>();
		System &sys = *system;
		systems.push_back(std::move(system));
		logger.log("System Created");

		return sys;
	}

	void checkEntity(const Entity &entity, const ComponentMask &mask)
	{
		// check if the provided entity fits into any of the registered systems
		for (auto &system : systems)
		{
			system->checkEntity(entity, mask);
		}
	}
};


#endif /* COMPONENTSYSTEMS_H */
