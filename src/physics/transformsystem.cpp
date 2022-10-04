#include "physics/transformsystem.h"
#include "core/componentstore.h"
#include "core/components/transformcomponent.h"
#include "physics/physicscomponent.h"

using namespace Boiler;

TransformSystem::TransformSystem() : System("Transform System")
{
	expects<TransformComponent>();
    expects<PhysicsComponent>();
}

void TransformSystem::update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, ComponentStore &store)
{
    for (const Entity &entity : getEntities())
	{
		TransformComponent &transform = store.retrieve<TransformComponent>(entity);
        PhysicsComponent &physics = store.retrieve<PhysicsComponent>(entity);

        transform.setPosition(transform.getPosition() + physics.velocity);
	}
}
