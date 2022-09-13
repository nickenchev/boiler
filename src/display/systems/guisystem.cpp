#include "core/componentstore.h"
#include "core/components/guicomponent.h"
#include "display/systems/guisystem.h"

using namespace Boiler;

void GUISystem::update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, ComponentStore &store)
{
	if (getEntities().size())
	{
		guiHandler->preRender(renderer);

		for (auto &entity : getEntities())
		{
			GUIComponent &guiComp = store.retrieve<GUIComponent>(entity);
			if (guiComp.guiRender)
			{
				guiComp.guiRender();
			}
		}
		guiHandler->render(renderer, frameInfo);
		guiHandler->postRender(renderer);
	}
}
