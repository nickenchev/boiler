#ifndef COLLISIONCOMPONENT_H
#define COLLISIONCOMPONENT_H

#include "core/componenttype.h"
#include "core/common.h"
#include "display/mesh.h"

namespace Boiler
{

enum class ColliderType
{
	AABB,
	Sphere,
	Mesh
};

struct CollisionComponent : public ComponentType<CollisionComponent>
{
	bool isDynamic = false;
	ColliderType colliderType = ColliderType::AABB;
	Mesh mesh;
	vec3 min, max;
	cgfloat damping = 0.2f;
	CollisionComponent() : ComponentType(this) {}
};

}

#endif // !COLLISIONCOMPONENT_H
