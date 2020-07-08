#include "video/systems/lightingsystem.h"
#include "core/components/lightingcomponent.h"
#include "core/componentstore.h"

using namespace Boiler;

void LightingSystem::update(Boiler::ComponentStore &store, const double delta)
{
	for (Entity entity : getEntities())
	{
		const LightingComponent &lightComp = store.retrieve<LightingComponent>(entity);
	}
}
