#include "video/systems/lightingsystem.h"
#include "core/components/lightingcomponent.h"
#include "core/componentstore.h"
#include "video/renderer.h"

using namespace Boiler;

void LightingSystem::update(Boiler::ComponentStore &store, const double)
{
	std::vector<LightSource> lightSources{};
	for (unsigned int i = 0; i < getEntities().size(); ++i)
	{
		LightingComponent &lightComp = store.retrieve<LightingComponent>(getEntities()[i]);
		lightSources.push_back(lightComp.source);
	}
	if (lightSources.size())
	{
		renderer.updateLights(lightSources);
	}
}
