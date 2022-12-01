#include "physics/physicssystem.h"
#include "physics/physicscomponent.h"
#include "physics/collisioncomponent.h"
#include "core/components/transformcomponent.h"
#include "core/entitycomponentsystem.h"
#include "core/matrixcache.h"
#include "core/components/rendercomponent.h"
#include "physics/aabb.h"

using namespace Boiler;

PhysicsSystem::PhysicsSystem(MatrixCache &matrixCache) : System("Physics System"), matrixCache(matrixCache)
{
	expects<TransformComponent>();
	expects<PhysicsComponent>();
	expects<CollisionComponent>();
}

cgfloat clamp(cgfloat value, cgfloat min, cgfloat max)
{
	return std::max(min, std::min(max, value));
}

void PhysicsSystem::update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, EntityComponentSystem &ecs)
{
	std::vector<Entity> bricksDestroyed(5);

	for (Entity entity : getEntities())
	{
		TransformComponent &transform = ecs.getComponentStore().retrieve<TransformComponent>(entity);
		PhysicsComponent &physics = ecs.getComponentStore().retrieve<PhysicsComponent>(entity);
		CollisionComponent &collision = ecs.getComponentStore().retrieve<CollisionComponent>(entity);

		const vec3 prevPos = transform.getPosition();
		transform.setPosition(transform.getPosition() + physics.velocity * frameInfo.deltaTime);

		// check dynamic bodies for collisions
		if (collision.isDynamic)
		{
			vec3 velocity = physics.velocity;

			// predict the transform after object is moved
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
						PhysicsComponent &physicsB = ecs.getComponentStore().retrieve<PhysicsComponent>(entityB);
						newTransformB.setPosition(newTransformB.getPosition() + physicsB.velocity * frameInfo.deltaTime);

						mat4 matB = matrixCache.getMatrix(entityB, ecs.getComponentStore());
						vec3 minB = vec3(matB * vec4(collisionB.min, 1));
						vec3 maxB = vec3(matB * vec4(collisionB.max, 1));

						if (AABB::intersects(minA, maxA, minB, maxB))
						{
							vec3 ballDir = transform.getPosition() - newTransformB.getPosition();
							vec3 normBallDir = glm::normalize(ballDir);
							/* p2 ---- p1
							      |  |
							   p3 ---- p4 */

							vec3 xHat(1, 0, 0);
							vec3 p1 = glm::normalize(vec3(maxB.x, minB.y, 0) - newTransformB.getPosition());
							vec3 p2 = glm::normalize(vec3(minB.x, minB.y, 0) - newTransformB.getPosition());

							cgfloat p1Theta = glm::acos(glm::dot(xHat, p1));
							cgfloat p2Theta = glm::pi<float>() - p1Theta;
							cgfloat ballTheta = glm::acos(glm::dot(xHat, normBallDir));

							if (ballTheta > p1Theta && ballTheta < p2Theta)
							{
								velocity.y = -velocity.y;
							}
							else
							{
								velocity.x = -velocity.x;
							}

							if (ecs.nameOf(entityB) == "Brick")
							{
								bricksDestroyed.push_back(entityB);
							}
							transform.setPosition(prevPos);
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
							transform.setPosition(prevPos);
						}
					}
				}
			}
			physics.velocity = velocity;
		}
	}

	for (Entity entity : bricksDestroyed)
	{
		ecs.removeComponent<RenderComponent>(entity);
		ecs.removeComponent<CollisionComponent>(entity);
	}
}
