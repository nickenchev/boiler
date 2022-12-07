#include "physics/physicssystem.h"
#include "physics/physicscomponent.h"
#include "physics/collisioncomponent.h"
#include "core/components/transformcomponent.h"
#include "core/entitycomponentsystem.h"
#include "core/matrixcache.h"
#include "core/components/rendercomponent.h"
#include "physics/aabb.h"
#include "physics/sphere.h"

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
	std::vector<Entity> bricksDestroyed;
	bricksDestroyed.reserve(5);

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
					mat4 matB = matrixCache.getMatrix(entityB, ecs.getComponentStore());
					vec3 minB = vec3(matB * vec4(collisionB.min, 1));
					vec3 maxB = vec3(matB * vec4(collisionB.max, 1));
					TransformComponent newTransformB = transformB;
					PhysicsComponent &physicsB = ecs.getComponentStore().retrieve<PhysicsComponent>(entityB);

					if (collision.colliderType == ColliderType::Sphere)
					{
						if (collisionB.colliderType == ColliderType::AABB)
						{
							vec3 size = maxA - minA;
							float radius = std::max(std::max(size.x, size.y), size.z) / 2.0f;

							if (Sphere::intersects(transform.getPosition(), radius, minB, maxB))
							{
								vec3 ballDir = transform.getPosition() - newTransformB.getPosition();
								vec3 normBallDir = glm::normalize(ballDir);
								/* p2 ---- p1
								   |  |
								   p3 ---- p4 */

								vec3 xHat(1, 0, 0);
								vec3 p1 = glm::normalize(vec3(maxB.x, minB.y, 0) - newTransformB.getPosition());
								vec3 p2 = glm::normalize(vec3(minB.x, minB.y, 0) - newTransformB.getPosition());

								float p1Theta = glm::acos(glm::dot(xHat, p1));
								float p2Theta = glm::pi<float>() - p1Theta;
								float ballTheta = glm::acos(glm::dot(xHat, normBallDir));

								if (ecs.nameOf(entityB) == "Paddle")
								{
									float d = std::clamp(glm::dot(normBallDir, xHat), -0.8f, 0.8f);
									logger.log("{}", d);
									if (d > -0.6f && d < 0.6f)
									{
										velocity.y = -velocity.y;
									}
									else
									{
										velocity = vec3(cos(acos(d)), sin(acos(d)), 0) * glm::length(velocity);
									}
								}
								else
								{
									if (ballTheta > p1Theta && ballTheta < p2Theta)
									{
										velocity.y = -velocity.y;
									}
									else
									{
										velocity.x = -velocity.x;
									}

									// remove the brick
									if (ecs.nameOf(entityB) == "Brick")
									{
										bricksDestroyed.push_back(entityB);
									}
								}
								transform.setPosition(prevPos);
							}
						}
						else if (collisionB.colliderType == ColliderType::AABB)
						{
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

								// bounce depending on mass / material
								if (physics.mass == 0)
								{
									if (ballTheta > p1Theta && ballTheta < p2Theta)
									{
										velocity.y = -velocity.y;
									}
									else
									{
										velocity.x = -velocity.x;
									}
								}
								if (ecs.nameOf(entityB) == "Brick")
								{
									bricksDestroyed.push_back(entityB);
								}
								transform.setPosition(prevPos);
							}
						}
					}
					else if (collision.colliderType == ColliderType::AABB)
					{
						if (collisionB.colliderType == ColliderType::AABB)
						{
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

								// bounce depending on mass / material
								if (physics.mass == 0)
								{
									if (ballTheta > p1Theta && ballTheta < p2Theta)
									{
										velocity.y = -velocity.y;
									}
									else
									{
										velocity.x = -velocity.x;
									}
								}
								if (ecs.nameOf(entityB) == "Brick")
								{
									bricksDestroyed.push_back(entityB);
								}
								transform.setPosition(prevPos);
							}
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
