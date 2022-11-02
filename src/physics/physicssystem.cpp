#include "physics/physicssystem.h"
#include "physics/physicscomponent.h"
#include "core/components/transformcomponent.h"
#include "core/entitycomponentsystem.h"

using namespace Boiler;

PhysicsSystem::PhysicsSystem() : System("Physics System")
{
	expects<TransformComponent>();
	expects<PhysicsComponent>();
}

void PhysicsSystem::update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, EntityComponentSystem &ecs)
{
	for (Entity entity : getEntities())
	{
		TransformComponent &transformComponent = ecs.getComponentStore().retrieve<TransformComponent>(entity);
		PhysicsComponent &physicsComponent = ecs.getComponentStore().retrieve<PhysicsComponent>(entity);
		transformComponent.setPosition(transformComponent.getPosition() + physicsComponent.velocity);
	}
}
