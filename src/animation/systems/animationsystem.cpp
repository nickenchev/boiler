#include "core/common.h"
#include "animation/animator.h"
#include "animation/systems/animationsystem.h"
#include "animation/components/animationcomponent.h"
#include "core/componentstore.h"

void Boiler::AnimationSystem::update(FrameInfo frameInfo, ComponentStore &store)
{
	for (const Entity &entity : getEntities())
	{
		auto &animationComp = store.retrieve<AnimationComponent>(entity);
		animator.animate(frameInfo.globalTime, frameInfo.deltaTime, animationComp);
	}
}
