#include "core/componentstore.h"
#include "core/components/guicomponent.h"
#include "display/systems/guisystem.h"

using namespace Boiler;

void GUISystem::update(ComponentStore &store, const Time, const Time)
{
	if (getEntities().size())
	{
		guiHandler->preRender();
		for (auto &entity : getEntities())
		{
			GUIComponent &guiComp = store.retrieve<GUIComponent>(entity);
			if (guiComp.guiRender)
			{
				guiComp.guiRender();
			}
		}
		guiHandler->render();
	}
}
