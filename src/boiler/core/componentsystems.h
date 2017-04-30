#ifndef COMPONENTSYSTEMS_H
#define COMPONENTSYSTEMS_H

#include <vector>
#include <memory>
#include "core/ecstypes.h"
#include "core/system.h"

class ComponentSystems
{
	std::vector<std::unique_ptr<System>> systems;
public:
    ComponentSystems() { }
    virtual ~ComponentSystems() { }

	void update(const double delta)
	{
		for (auto &system : systems)
		{
			system->update(delta);
		}
	}

	template<class T>
	System &createSystem()
	{
		auto system = std::make_unique<T>();
		systems.push_back(std::move(system));

		return *system;
	}
};


#endif /* COMPONENTSYSTEMS_H */
