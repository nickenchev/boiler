#include "core/componentstore.h"
#include "core/components/guicomponent.h"
#include "video/systems/guisystem.h"

using namespace Boiler;

void GUISystem::update(ComponentStore &store, const double delta) const
{
	if (getEntities().size())
	{
		guiHandler->preRender();
		for (auto &entity : getEntities())
		{
			GUIComponent &guiComp = store.retrieve<GUIComponent>(entity);
			if (guiComp.guiRender) {
				guiComp.guiRender();
			}
		}
		guiHandler->render();
	}
}
