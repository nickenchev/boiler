#include "display/systems/lightingsystem.h"
#include "core/components/lightingcomponent.h"
#include "core/componentstore.h"
#include "display/renderer.h"

using namespace Boiler;

LightingSystem::LightingSystem(Renderer &renderer) : System("Lighting System"), renderer(renderer)
{
	expects<LightingComponent>();
}

void LightingSystem::update(const FrameInfo &frameInfo, ComponentStore &store)
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
