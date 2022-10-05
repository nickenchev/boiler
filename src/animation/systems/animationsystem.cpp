#include "core/common.h"
#include "animation/animator.h"
#include "animation/systems/animationsystem.h"
#include "animation/components/animationcomponent.h"
#include "core/entitycomponentsystem.h"

using namespace Boiler;

AnimationSystem::AnimationSystem(Animator &animator) : System("Animation System"), animator(animator)
{
	expects<AnimationComponent>();
	expects<TransformComponent>();
}

void AnimationSystem::update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, EntityComponentSystem &ecs)
{
	for (const Entity &entity : getEntities())
	{
        auto &animationComp = ecs.getComponentStore().retrieve<AnimationComponent>(entity);
		animator.animate(frameInfo.globalTime, frameInfo.deltaTime, animationComp);
	}
}
