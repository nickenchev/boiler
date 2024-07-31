#include "core/entitycomponentsystem.h"

using namespace Boiler;

EntityComponentSystem::EntityComponentSystem() : logger("ECS"), systems(entityWorld)
{
}

void EntityComponentSystem::endFrame()
{
	// remove collisions
	for (Entity entity : getEntityWorld().getEntities())
	{
		if (getComponentStore().hasComponent<CollisionComponent>(entity))
		{
			CollisionComponent &collision = retrieve<CollisionComponent>(entity);
			removeComponentImmediate<CollisionComponent>(entity);
		}
	}

	// deferred component removals
	for (;!removeComponents.empty(); removeComponents.pop())
	{
		const auto &removable = removeComponents.front();
		Entity entity = std::get<0>(removable);
		ComponentMask mask = std::get<1>(removable);
		unsigned int storageIndex = std::get<2>(removable);
		removeComponent(entity, mask, storageIndex);
	}

	// check any entities that have been modified
	for (;!deferredEntityChecks.empty(); deferredEntityChecks.pop())
	{
		const auto &item = deferredEntityChecks.front();
		systems.checkEntity(std::get<Entity>(item), std::get<ComponentMask>(item));
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

void EntityComponentSystem::setName(Entity entity, const std::string &name)
{
	return entityWorld.setName(entity, name);
}

void EntityComponentSystem::removeEntity(const Entity &entity)
{
	throw std::runtime_error("Unimplemented");
}
