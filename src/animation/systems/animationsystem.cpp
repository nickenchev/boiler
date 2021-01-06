#include "animation/animator.h"
#include "animation/systems/animationsystem.h"
#include "animation/components/animationcomponent.h"
#include "core/componentstore.h"

void Boiler::AnimationSystem::update(ComponentStore &store, const double delta)
{
	totalTime += delta;

	for (const Entity &entity : getEntities())
	{
		const auto &animationComp = store.retrieve<AnimationComponent>(entity);
		animator.animate(delta, animationComp);
	}
}
