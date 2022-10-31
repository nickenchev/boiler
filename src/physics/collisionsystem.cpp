#include "core/componentstore.h"
#include "physics/collisionsystem.h"
#include "physics/collisioncomponent.h"
#include "core/components/transformcomponent.h"
#include "physics/physicscomponent.h"
#include "core/entitycomponentsystem.h"
#include "display/renderer.h"
#include "core/matrixcache.h"

using namespace Boiler;

CollisionSystem::CollisionSystem(MatrixCache &matrixCache) : System("Collision System"), matrixCache(matrixCache)
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

		// check dynamic bodies for collisions
		if (collision.isDynamic)
		{
			vec3 velocity = physics.velocity;

			// predict the transform after object is moved
			TransformComponent newTransformA = transform;
			newTransformA.setPosition(newTransformA.getPosition() * frameInfo.deltaTime);

			mat4 matA = matrixCache.getMatrix(entity, ecs.getComponentStore());
			vec3 minA = vec3(matA * vec4(collision.min, 1));
			vec3 maxA = vec3(matA * vec4(collision.max, 1));

			for (const Entity &entityB : getEntities())
			{
				if (entity != entityB)
				{
					auto &collisionB = ecs.getComponentStore().retrieve<CollisionComponent>(entityB);
					auto &transformB = ecs.getComponentStore().retrieve<TransformComponent>(entityB);

					if (collisionB.colliderType == ColliderType::AABB)
					{
						TransformComponent newTransformB = transformB;
						if (ecs.getComponentStore().hasComponent<PhysicsComponent>(entityB))
						{
							PhysicsComponent &physicsB = ecs.getComponentStore().retrieve<PhysicsComponent>(entityB);
							newTransformB.setPosition(newTransformB.getPosition() * frameInfo.deltaTime);
						}

						mat4 matB = matrixCache.getMatrix(entityB, ecs.getComponentStore());
						vec3 minB = vec3(matB * vec4(collisionB.min, 1));
						vec3 maxB = vec3(matB * vec4(collisionB.max, 1));

						if (maxA.x > minB.x && minA.x < maxB.x &&
							maxA.y > minB.y && minA.y < maxB.y &&
							maxA.z > minB.z && minA.z < maxB.z)
						{
							velocity = -velocity * 0.1f;
						}
					}
					else if (collisionB.colliderType == ColliderType::Sphere)
					{
						vec3 minB = vec3(transformB.getMatrix() * vec4(collisionB.min, 1));
						vec3 maxB = vec3(transformB.getMatrix() * vec4(collisionB.max, 1));

						cgfloat diameterA = glm::length(minA - maxA);
						cgfloat radiusA = diameterA / 2;
						cgfloat diameterB = glm::length(minB - maxB);
						cgfloat radiusB = diameterB / 2;

						cgfloat distance = glm::distance(transform.getPosition() + velocity * frameInfo.deltaTime, transformB.getPosition());

						if (distance < radiusA + radiusB)
						{
							velocity = -velocity * 0.1f;
						}
					}
				}
			}
			physics.velocity = velocity;
		}
	}
}
