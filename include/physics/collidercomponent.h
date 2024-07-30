#ifndef COLLIDERCOMPONENT_H
#define COLLIDERCOMPONENT_H

#include "core/componenttype.h"
#include "core/common.h"
#include "display/mesh.h"

namespace Boiler
{

enum class ColliderType
{
	AABB,
	Sphere,
	Mesh,
	None
};

struct ColliderComponent : public ComponentType<ColliderComponent>
{
	bool isDynamic = false;
	ColliderType colliderType = ColliderType::AABB;
	vec3 normal = vec3(0, 0, 0);
	AssetId meshId;
	vec3 min, max;
	cgfloat damping = 0.2f;
	ColliderComponent() : ComponentType(this) {}
};

}

#endif /* COLLIDERCOMPONENT_H */
