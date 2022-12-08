#include "core/entitycomponentsystem.h"

using namespace Boiler;

EntityComponentSystem::EntityComponentSystem() : logger("ECS")
{
	removeComponents.reserve(100);
}

void EntityComponentSystem::endFrame()
{
	// deferred component removals
	for (auto deferred : removeComponents)
	{
		Entity entity = std::get<0>(deferred);
		ComponentMask mask = std::get<1>(deferred);
		unsigned int storageIndex = std::get<2>(deferred);
		removeComponent(entity, mask, storageIndex);
	}
	removeComponents.clear();

	// remove collisions
	for (Entity entity : getEntityWorld().getEntities())
	{
		if (getComponentStore().hasComponent<CollisionComponent>(entity))
		{
			CollisionComponent &collision = retrieve<CollisionComponent>(entity);
			removeComponentImmediate<CollisionComponent>(entity);
		}
	}
}

void EntityComponentSystem::removeComponent(const Entity &entity, ComponentMask mask, unsigned int storageIndex)
{
	auto entMask = mapper.remove(entity, mask);
	componentStore.remove(entity, storageIndex);
	systems.checkEntity(entity, entMask);
}

void EntityComponentSystem::update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, SystemStage stage)
{
	systems.update(renderer, assetSet, frameInfo, stage, *this);
}

Entity EntityComponentSystem::newEntity(const std::string &name)
{
	return entityWorld.createEntity(name);
}

const std::string &EntityComponentSystem::nameOf(Entity entity)
{
	return entityWorld.getName(entity);
}

void EntityComponentSystem::removeEntity(const Entity &entity)
{
	throw std::runtime_error("Unimplemented");
	logger.log("Deleting entity #: " + std::to_string(entity.getId()));

	/*
	// find children and remove
	auto children = componentStore.find<ParentComponent>();
	for (auto child : children)
	{
	ParentComponent &parent = componentStore.retrieve<ParentComponent>(child);
	if (parent.entity == entity)
	{
	removeEntity(child);
	}
	}

	// remove entity itself
	systems.removeEntity(entity);
	componentStore.removeAll(entity);
	entityWorld.removeEntity(entity);
	*/
}
