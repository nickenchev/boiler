#ifndef COMPONENTSYSTEMS_H
#define COMPONENTSYSTEMS_H

#include <vector>
#include <memory>
#include "core/ecstypes.h"
#include "core/system.h"
#include "core/logger.h"
#include "core/componentmapper.h"
#include "core/componentstore.h"

class ComponentSystems
{
	Logger logger;
	std::vector<std::unique_ptr<System>> systems;
	std::vector<System *> updateSystems;

public:
    ComponentSystems() : logger{"ComponentSystems"} { }
    virtual ~ComponentSystems() { }

	void update(ComponentStore &store, const double delta)
	{
		for (auto &system : updateSystems)
		{
			system->update(store, delta);
		}
	}

	template<typename T, typename... Args>
	System &registerSystem(Args&&... args)
	{
		// TODO: Check if any of the existing entities fit into the newly registered system
		auto system = std::make_unique<T>(std::forward<Args>(args)...);
		System &sys = *system;
		systems.push_back(std::move(system));
		updateSystems.push_back(&sys);
		logger.log("System Registered: " + sys.name);

		return sys;
	}

	void removeUpdate(const System *system)
	{
		auto itr = std::find(updateSystems.begin(), updateSystems.end(), system);
		if (itr != updateSystems.end())
		{
			updateSystems.erase(itr);
			logger.log("Removed " + system->name + " from update list.");
		}
		else
		{
			logger.error("Error finding system in update list.");
		}
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
