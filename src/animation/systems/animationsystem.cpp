#include "animation/systems/animationsystem.h"

void Boiler::AnimationSystem::update(ComponentStore &store, const double delta)
{
	totalTime += delta;

	for (const Entity &entity : getEntities())
	{
	}
}
