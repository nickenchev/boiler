#ifndef COLLISIONCOMPONENT_H
#define COLLISIONCOMPONENT_H

#include "core/componenttype.h"

namespace Boiler
{
	class CollisionComponent : public ComponentType<CollisionComponent>
	{
	public:
		CollisionComponent() : ComponentType(this) { }
	};
}

#endif // !COLLISIONCOMPONENT_H
