#ifndef COMPONENTSYSTEMS_H
#define COMPONENTSYSTEMS_H

#include <vector>
#include <memory>
#include "core/ecstypes.h"
#include "core/system.h"
#include "core/logger.h"

class ComponentSystems
{
	Logger logger;
	std::vector<std::unique_ptr<System>> systems;

public:
    ComponentSystems() : logger{"ComponentSystems"} { }
    virtual ~ComponentSystems() { }

	void update(const double delta)
	{
		for (auto &system : systems)
		{
			system->update(delta);
		}
	}

	template<class T>
	System &registerSystem()
	{
		auto system = std::make_unique<T>();
		System &sys = *system;
		systems.push_back(std::move(system));

		logger.log("Created system");

		return sys;
	}
};


#endif /* COMPONENTSYSTEMS_H */
