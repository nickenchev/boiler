#include "core/componentstore.h"
#include "collision/collisionsystem.h"
#include "collision/collisioncomponent.h"
#include "core/components/transformcomponent.h"

using namespace Boiler;
#define GROUND_Y 1.1f

void CollisionSystem::update(FrameInfo frameInfo, ComponentStore &store)
{
	for (Entity entity : getEntities())
	{
		vec3 gravity(0, -0.098f, 0);

		auto &transform = store.retrieve<TransformComponent>(entity);
		vec3 position = transform.getPosition();
		vec3 acceleration = transform.getAcceleration();
		acceleration += gravity * frameInfo.getDeltaTime();
		position += acceleration;

		if (position.y <= GROUND_Y)
		{
			position.y = GROUND_Y;
			acceleration.y += -acceleration.y * 1.5f;
		}
		transform.setPosition(position);
		transform.setAcceleration(acceleration);

		// recalculate matrix and transform collision component
		mat4 transformMatrix = transform.getMatrix();
		auto &collision = store.retrieve<CollisionComponent>(entity);
		vec4 min = vec4(collision.min, 1) * transformMatrix;
		vec4 max = vec4(collision.max, 1) * transformMatrix;
	}
}
