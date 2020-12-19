#ifndef COMPONENTSTORE_H
#define COMPONENTSTORE_H

#include <cassert>
#include <vector>
#include <unordered_map>
#include <memory>
#include "core/component.h"
#include "core/entity.h"
#include "core/logger.h"

namespace Boiler
{

constexpr unsigned short MAX_COMPONENTS = 128;
constexpr unsigned int MAX_ENTITIES = 1000;

class ComponentStore
{
	Logger logger;
	using Components = std::array<std::shared_ptr<Component>, MAX_COMPONENTS>;
	std::array<Components, MAX_ENTITIES> entityComponents;

public:
	ComponentStore() : logger("Component Store")
	{
	}

	inline size_t index(const Entity entity) const
	{
		return entity.getId() - 1;
	}

	template<typename T, typename... Args>
	std::shared_ptr<T> store(const Entity &entity, Args&&... args)
	{
		// construct and store the new compnent for this entity
		auto component = std::make_shared<T>(std::forward<Args>(args)...);
		entityComponents[index(entity)][T::mask.to_ulong()] = component;

		return component;
	}

	template<typename T>
	void remove(const Entity &entity)
	{
		entityComponents[index(entity)][T::mask.to_ulong()] = nullptr;
	}

	void removeAll(const Entity &entity)
	{
		// remove all components first and then the entity
		/*
		ComponentMap &compMap = entityComponents[entity.getId()];
		for (auto pair : compMap)
		{
			compMap.erase(pair.first);
		}
		entityComponents.erase(entity.getId());
		*/
	}

	template<typename T>
	T &retrieve(const Entity &entity)
	{
		auto component = entityComponents[index(entity)][T::mask.to_ulong()];

		assert(component != nullptr);
		return *std::static_pointer_cast<T>(component);
	}

	template<typename T>
	bool hasComponent(const Entity &entity)
	{
		return entityComponents[index(entity)][T::mask.to_ulong()] != nullptr;
	}

	template<typename T>
	auto find()
	{
		/*
		std::vector<Entity> list;
		for (auto pair : entityComponents)
		{
			auto comp = pair.second[T::mask];
			if (comp != nullptr)
			{
				list.push_back(pair.first);
			}
		}
		return list;
		*/
	}

	auto &copyComponents(const Entity source, const Entity destination)
	{
		for (auto &component : entityComponents[index(source)])
		{
			if (component != nullptr)
			{
				logger.log("Component with mask: {}", component->getMask().to_string());
				auto newComponent = std::shared_ptr<Component>(component->clone());
				entityComponents[index(destination)][component->getMask().to_ulong()] = newComponent;
			}
		}
		return entityComponents[index(destination)];
	}
};

}

#endif /* COMPONENTSTORE_H */
