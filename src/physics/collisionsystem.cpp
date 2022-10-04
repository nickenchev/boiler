#include "core/componentstore.h"
#include "physics/collisionsystem.h"
#include "physics/collisioncomponent.h"
#include "core/components/transformcomponent.h"
#include "physics/physicscomponent.h"

using namespace Boiler;

CollisionSystem::CollisionSystem() : System("Collision System")
{
	expects<CollisionComponent>();
	expects<TransformComponent>();
	expects<PhysicsComponent>();
}

void CollisionSystem::update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, ComponentStore &store)
{
	float ground = 10;
	float radius = 3;

	for (Entity entity : getEntities())
	{
		auto &collision = store.retrieve<CollisionComponent>(entity);
		auto &transform = store.retrieve<TransformComponent>(entity);
		auto &physics = store.retrieve<PhysicsComponent>(entity);

		logger.log("{}", entity.getId());
		// check dynamic bodies for collisions
		if (collision.isDynamic)
		{
			vec3 position = transform.getPosition();
			vec3 velocity = physics.velocity;

			mat4 matA = transform.getMatrix();
			vec3 minA = (collision.min + transform.getPosition() + velocity) * transform.getScale();
			vec3 maxA = (collision.max + transform.getPosition() + velocity) * transform.getScale();

			// need transform info to respond to collisions

			//logger.log("Scanning... {}, {}, {}    {}, {}, {}", minA.x, minA.y, minA.z, maxA.x, maxA.y, maxA.z);
			for (Entity entityB : getEntities())
			{
				if (entity != entityB)
				{
					auto &collisionB = store.retrieve<CollisionComponent>(entityB);
					auto &transformB = store.retrieve<TransformComponent>(entityB);

					mat4 matB = transformB.getMatrix();
					vec3 minB = collision.min + transformB.getPosition();
					vec3 maxB = collision.max + transformB.getPosition();

					logger.log("{}: ({}, {}, {})   ({}, {}, {})", entity.getId(), minA.x, minA.y, minA.z, maxA.x, maxA.y, maxA.z);
					logger.log("{}: ({}, {}, {})   ({}, {}, {})", entityB.getId(), minB.x, minB.y, minB.z, maxB.x, maxB.y, maxB.z);
					if (store.hasComponent<PhysicsComponent>(entityB))
					{
						PhysicsComponent &physicsB = store.retrieve<PhysicsComponent>(entityB);
						minB += physicsB.velocity;
						maxB += physicsB.velocity;
					}
					minB *= transformB.getScale();
					maxB *= transformB.getScale();

					if (maxA.x > minB.x &&
						minA.x < maxB.x &&
						maxA.y > minB.y &&
						minA.y < maxB.y &&
						maxA.z > minB.z &&
						minA.z < maxB.z)
					{
						velocity.y = 0;
					}
				}
			}
			physics.velocity = velocity;
		}
	}
}
