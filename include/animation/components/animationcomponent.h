#ifndef ANIMATIONCOMPONENT_H
#define ANIMATIONCOMPONENT_H

#include <vector>
#include "core/componenttype.h"
#include "animation/clip.h"

namespace Boiler
{

class AnimationComponent : public ComponentType<AnimationComponent>
{
	// TODO: Check if vector causes runtime issues due to alloc
	const std::vector<Entity> targets;
	std::vector<Clip> clips;

public:
	AnimationComponent() : ComponentType(this) { }
	AnimationComponent(const std::vector<Entity> &targets) : ComponentType(this), targets(targets) { }

	const auto &getTargets() const { return targets; }

	void addClip(const Clip &clip)
	{
		clips.push_back(clip);
	}

	auto &getClips() { return clips; }
};

};

#endif /* ANIMATIONCOMPONENT_H */
