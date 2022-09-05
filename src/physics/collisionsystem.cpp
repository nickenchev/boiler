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

void CollisionSystem::update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, ComponentStore &store)
{
	float ground = 1.3;
	float radius = 3;

	for (Entity entity : getEntities())
	{
		auto &collision = store.retrieve<CollisionComponent>(entity);
		// check dynamic bodies for collisions
		if (collision.isDynamic)
		{
			// need transform info to respond to collisions
			auto &transform = store.retrieve<TransformComponent>(entity);
			vec3 position = transform.getPosition();
			vec3 velocity = transform.getVelocity();

			if (position.y < ground)
			{
				position.y = ground;
				velocity.y = -velocity.y * 0.1f;
			}

			transform.setVelocity(velocity);
			transform.setPosition(position);
		}
		//vec4 min = vec4(collision.min, 1) * transformMatrix;
		//vec4 max = vec4(collision.max, 1) * transformMatrix;
	}
}
