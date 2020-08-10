#include "video/systems/lightingsystem.h"
#include "core/components/lightingcomponent.h"
#include "core/componentstore.h"
#include "video/renderer.h"

using namespace Boiler;

void LightingSystem::update(Boiler::ComponentStore &store, const double delta)
{
	for (unsigned int i = 0; i < getEntities().size(); ++i)
	{
		LightingComponent &lightComp = store.retrieve<LightingComponent>(getEntities()[i]);
		renderer.addLightSource(lightComp.source);
		lightComp.source.position.y += 3.0f * delta;
	}
}
