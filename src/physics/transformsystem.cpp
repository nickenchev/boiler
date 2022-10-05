#include "physics/transformsystem.h"
#include "core/entitycomponentsystem.h"
#include "core/components/transformcomponent.h"
#include "physics/physicscomponent.h"

using namespace Boiler;

TransformSystem::TransformSystem() : System("Transform System")
{
	expects<TransformComponent>();
    expects<PhysicsComponent>();
}

void TransformSystem::update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, EntityComponentSystem &ecs)
{
    for (const Entity &entity : getEntities())
	{
        TransformComponent &transform = ecs.getComponentStore().retrieve<TransformComponent>(entity);
        PhysicsComponent &physics = ecs.getComponentStore().retrieve<PhysicsComponent>(entity);

        transform.setPosition(transform.getPosition() + physics.velocity * frameInfo.deltaTime);
	}
}
