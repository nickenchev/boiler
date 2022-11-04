#ifndef COMPONENTSYSTEMS_H
#define COMPONENTSYSTEMS_H

#include <vector>
#include <memory>
#include <algorithm>
#include "core/ecstypes.h"
#include "core/system.h"
#include "core/logger.h"

namespace Boiler
{

enum class SystemStage
{
	PRE_RENDER,
	RENDER
};

class EntityComponentSystem;

class ComponentSystems
{
	Logger logger;
	std::vector<std::unique_ptr<System>> systems;
	std::vector<System *> preRenderSystems, renderSystems;

public:
    ComponentSystems() : logger{"Component Systems"} { }
    virtual ~ComponentSystems() { }

    void update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, SystemStage stage, EntityComponentSystem &ecs)
	{
		if (stage == SystemStage::PRE_RENDER)
		{
			for (auto &system : preRenderSystems)
			{
				system->update(renderer, assetSet, frameInfo, ecs);
			}
		}
		else if (stage == SystemStage::RENDER)
		{
			for (auto &system : renderSystems)
			{
				system->update(renderer, assetSet, frameInfo, ecs);
			}
		}
	}

	template<typename T, typename... Args>
	T &registerSystem(SystemStage stage, Args&&... args)
	{
		static_assert(std::is_base_of<System, T>(), "Specified type is not a System");

		// TODO: Check if any of the existing entities fit into the newly registered system
		auto system = std::make_unique<T>(std::forward<Args>(args)...);
		T &sysRef = static_cast<T &>(*system);
		systems.push_back(std::move(system));

		if (stage == SystemStage::PRE_RENDER)
		{
			preRenderSystems.push_back(&sysRef);
		}
		else if (stage == SystemStage::RENDER)
		{
			renderSystems.push_back(&sysRef);
		}

		logger.log("System Registered: {}", sysRef.getName());

		return sysRef;
	}

	void checkEntity(const Entity &entity, const ComponentMask &mask)
	{
		// check if the provided entity fits into any of the registered systems
		for (auto &system : systems)
		{
			system->checkEntity(entity, mask);
		}
	}

	void removeEntity(const Entity &entity)
	{
		for (auto &system : systems)
		{
			system->removeEntity(entity);
		}
	}
};

}

#endif /* COMPONENTSYSTEMS_H */
