#ifndef COMPONENTSTORE_H
#define COMPONENTSTORE_H

#include <cassert>
#include <array>
#include <unordered_map>
#include <memory>
#include "core/component.h"
#include "core/entity.h"
#include "core/logger.h"

namespace Boiler
{

constexpr unsigned short MAX_COMPONENTS = 64;

class ComponentStore
{
	Logger logger;
	using Components = std::array<std::unique_ptr<Component>, MAX_COMPONENTS>;
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
	T &store(const Entity &entity, Args&&... args)
	{
		// construct and store the new compnent for this entity
		unsigned int entityIndex = index(entity);
		unsigned int storageIndex = T::storageIndex;
		assert(entityIndex < entityComponents.size());

		entityComponents[entityIndex][T::storageIndex] = std::make_unique<T>(std::forward<Args>(args)...);
		T *ptr = static_cast<T*>(entityComponents[entityIndex][T::storageIndex].get());

		return *ptr;
	}

	template<typename T>
	void remove(const Entity &entity)
	{
		entityComponents[index(entity)][T::storageIndex] = nullptr;
	}

	void removeAll(const Entity &)
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
		auto &component = entityComponents[index(entity)][T::storageIndex];

		assert(component != nullptr);
		return *static_cast<T*>(component.get());
	}

	template<typename T>
	bool hasComponent(const Entity &entity)
	{
		return entityComponents[index(entity)][T::storageIndex] != nullptr;
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
};

}

#endif /* COMPONENTSTORE_H */
