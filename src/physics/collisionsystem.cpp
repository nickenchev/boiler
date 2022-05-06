#include "core/componentstore.h"
#include "physics/collisionsystem.h"
#include "physics/collisioncomponent.h"
#include "core/components/transformcomponent.h"

using namespace Boiler;
#define GROUND_Y 1.1f

void CollisionSystem::update(FrameInfo frameInfo, ComponentStore &store)
{
	for (Entity entity : getEntities())
	{
		const float gravity = 9.8f;

		auto &transform = store.retrieve<TransformComponent>(entity);
		mat4 transformMatrix = transform.getMatrix();

		// recalculate matrix and transform collision component
		auto &collision = store.retrieve<CollisionComponent>(entity);
		vec4 min = vec4(collision.min, 1) * transformMatrix;
		vec4 max = vec4(collision.max, 1) * transformMatrix;

		vec3 position = transform.getPosition();
		vec3 velocity = transform.getVelocity();

		velocity.y -= gravity * frameInfo.deltaTime;
		if (position.y < GROUND_Y)
		{
			position.y = GROUND_Y;
			velocity.y = -velocity.y * 0.1f;
		}

		position += velocity * frameInfo.deltaTime;
		transform.setPosition(position);
		transform.setVelocity(velocity);
	}
}
