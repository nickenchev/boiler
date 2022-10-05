#include "core/componentstore.h"
#include "physics/collisionsystem.h"
#include "physics/collisioncomponent.h"
#include "core/components/transformcomponent.h"
#include "physics/physicscomponent.h"
#include "core/entitycomponentsystem.h"

using namespace Boiler;

CollisionSystem::CollisionSystem() : System("Collision System")
{
	expects<CollisionComponent>();
	expects<TransformComponent>();
	expects<PhysicsComponent>();
}

void CollisionSystem::update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, EntityComponentSystem &ecs)
{
    for (const Entity &entity : getEntities())
	{
        auto &collision = ecs.getComponentStore().retrieve<CollisionComponent>(entity);
        auto &transform = ecs.getComponentStore().retrieve<TransformComponent>(entity);
        auto &physics = ecs.getComponentStore().retrieve<PhysicsComponent>(entity);

		// generate debug geometry
		

		// check dynamic bodies for collisions
		if (collision.isDynamic)
		{
            vec3 velocity = physics.velocity;

			// predict the transform after object is moved
			TransformComponent newTransformA = transform;
			newTransformA.setPosition(newTransformA.getPosition() + physics.velocity * frameInfo.deltaTime);

			vec3 minA = vec3(newTransformA.getMatrix() * vec4(collision.min, 1));
			vec3 maxA = vec3(newTransformA.getMatrix() * vec4(collision.max, 1));
			logger.log("{}: ({}, {}, {})   ({}, {}, {})", ecs.nameOf(entity), minA.x, minA.y, minA.z, maxA.x, maxA.y, maxA.z);

            for (const Entity &entityB : getEntities())
			{
				if (entity != entityB)
				{
                    auto &collisionB = ecs.getComponentStore().retrieve<CollisionComponent>(entityB);
                    auto &transformB = ecs.getComponentStore().retrieve<TransformComponent>(entityB);

					TransformComponent newTransformB = transformB;
                    if (ecs.getComponentStore().hasComponent<PhysicsComponent>(entityB))
					{
                        PhysicsComponent &physicsB = ecs.getComponentStore().retrieve<PhysicsComponent>(entityB);
						newTransformB.setPosition(newTransformB.getPosition() + physicsB.velocity * frameInfo.deltaTime);
					}
					vec3 minB = vec3(newTransformB.getMatrix() * vec4(collisionB.min, 1));
					vec3 maxB = vec3(newTransformB.getMatrix() * vec4(collisionB.max, 1));

					if (maxA.x > minB.x && minA.x < maxB.x &&
						maxA.y > minB.y && minA.y < maxB.y &&
						maxA.z > minB.z && minA.z < maxB.z)
					{
						logger.log("{}: ({}, {}, {})   ({}, {}, {})", ecs.nameOf(entityB), minB.x, minB.y, minB.z, maxB.x, maxB.y, maxB.z);
						velocity *= 0;
					}
				}
			}
			physics.velocity = velocity;
		}
	}
}
