#ifndef COLLISIONCOMPONENT_H
#define COLLISIONCOMPONENT_H

#include "core/componenttype.h"
#include "core/common.h"
#include "display/mesh.h"

namespace Boiler
{

enum class ColliderType
{
	None,
	AABB,
	Mesh
};

struct CollisionComponent : public ComponentType<CollisionComponent>
{
	bool isDynamic = false;
	ColliderType colliderType = ColliderType::None;
	Mesh mesh;
	vec3 min, max;
	CollisionComponent() : ComponentType(this) {}
};

}

#endif // !COLLISIONCOMPONENT_H
