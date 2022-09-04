#include "display/systems/lightingsystem.h"
#include "core/components/lightingcomponent.h"
#include "core/componentstore.h"
#include "display/renderer.h"

using namespace Boiler;

LightingSystem::LightingSystem() : System("Lighting System")
{
	expects<LightingComponent>();
}

void LightingSystem::update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, ComponentStore &store)
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
