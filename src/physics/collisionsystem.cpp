#include "core/componentstore.h"
#include "physics/collisionsystem.h"
#include "physics/collisioncomponent.h"
#include "core/components/transformcomponent.h"

using namespace Boiler;

CollisionSystem::CollisionSystem() : System("Collision System")
{
	expects<CollisionComponent>();
	expects<TransformComponent>();
}

void CollisionSystem::update(const FrameInfo &frameInfo, ComponentStore &store)
{
	for (Entity entity : getEntities())
	{
		auto &collision = store.retrieve<CollisionComponent>(entity);
		//vec4 min = vec4(collision.min, 1) * transformMatrix;
		//vec4 max = vec4(collision.max, 1) * transformMatrix;
	}
}
