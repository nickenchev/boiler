#include "physics/physicssystem.h"
#include "physics/physicscomponent.h"
#include "physics/collisioncomponent.h"
#include "core/components/transformcomponent.h"
#include "core/entitycomponentsystem.h"
#include "core/matrixcache.h"
#include "core/components/rendercomponent.h"

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

						/*
						if (collision.colliderType == ColliderType::Sphere)
						{
							vec3 halfA = (maxA - minA) / 2.0f;
							cgfloat radius = std::max(std::max(halfA.x, halfA.y), halfA.z);

							// get vector pointing from box centre to sphere center
							vec3 d = transform.getPosition() - newTransformB.getPosition();

							vec3 halfAABB = vec3(maxB.x - minB.x, maxB.y - minB.y, maxB.z - minB.z) / 2.0f;
							vec3 clamped = vec3(clamp(d.x, -halfAABB.x, halfAABB.x),
												clamp(d.y, -halfAABB.y, halfAABB.y),
												clamp(d.z, -halfAABB.z, halfAABB.z));

							logger.log("{}  {}", glm::length(clamped), radius);
							if (glm::length(clamped) < radius)
							{
								logger.log("HIT {}", ecs.nameOf(entityB));
							}

						}
						*/

						if (maxA.x > minB.x && minA.x < maxB.x &&
							maxA.y > minB.y && minA.y < maxB.y &&
							maxA.z > minB.z && minA.z < maxB.z)
						{
							vec3 up(0, 1, 0);
							vec3 left(1, 0, 0);
							vec3 ballDir = glm::normalize(transform.getPosition() - newTransformB.getPosition());
							cgfloat dUp = glm::dot(up, ballDir);
							cgfloat dLeft = glm::dot(left, ballDir);
							cgfloat wRatio = (maxB.x - minB.x) / (maxB.y - minB.y);
							cgfloat hRatio = (maxB.y - minB.y) / (maxB.x - minB.x);

							logger.log("{}: ({}, {}, {}) -- {}: ({}, {}, {}) -- dUp: {}, dLeft: {}", ecs.nameOf(entity), transform.getPosition().x,
									   transform.getPosition().y, transform.getPosition().z, ecs.nameOf(entityB), newTransformB.getPosition().x,
									   newTransformB.getPosition().y, newTransformB.getPosition().z, dUp, dLeft);

							if (ecs.nameOf(entityB) == "Brick")
							{
								bricksDestroyed.push_back(entityB);
							}
							
							transform.setPosition(prevPos);

							if (abs(dUp * wRatio) > abs(dLeft * hRatio))
							{
								velocity.x = -velocity.x * collision.damping;
							}
							else
							{
								velocity.y = -velocity.y * collision.damping;
							}
							velocity = -velocity * collision.damping;
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
