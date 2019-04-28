#include "core/componentstore.h"
#include "core/components/guicomponent.h"
#include "video/systems/guisystem.h"

void GUISystem::update(ComponentStore &store, const double delta) const
{
	guiHandler->preRender();
	for (auto &entity : getEntities())
	{
		GUIComponent &guiComp = store.retrieve<GUIComponent>(entity);
		guiComp.guiRender();
	}
	guiHandler->render();
}
