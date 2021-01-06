#ifndef ANIMATIONCOMPONENT_H
#define ANIMATIONCOMPONENT_H

#include <vector>
#include "core/componenttype.h"

namespace Boiler
{

class AnimationComponent : public ComponentType<AnimationComponent>
{
	const std::vector<Entity> targets;

public:
	AnimationComponent() : ComponentType(this) { }
	AnimationComponent(const std::vector<Entity> &targets) : ComponentType(this), targets(targets) { }
};

};

#endif /* ANIMATIONCOMPONENT_H */
