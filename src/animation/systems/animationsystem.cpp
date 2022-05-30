#include "core/common.h"
#include "animation/animator.h"
#include "animation/systems/animationsystem.h"
#include "animation/components/animationcomponent.h"
#include "core/componentstore.h"

using namespace Boiler;

AnimationSystem::AnimationSystem(Animator &animator) : System("Animation System"), animator(animator)
{
	expects<AnimationComponent>();
	expects<TransformComponent>();
}

void AnimationSystem::update(AssetSet &assetSet, const FrameInfo &frameInfo, ComponentStore &store)
{
	for (const Entity &entity : getEntities())
	{
		auto &animationComp = store.retrieve<AnimationComponent>(entity);
		animator.animate(frameInfo.globalTime, frameInfo.deltaTime, animationComp);
	}
}
