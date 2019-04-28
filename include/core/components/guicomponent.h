#ifndef GUICOMPONENT_H
#define GUICOMPONENT_H

#include <functional>
#include "core/componenttype.h"

struct GUIComponent : public ComponentType<GUIComponent>
{
	const std::function<void()> guiRender;
	
	GUIComponent(std::function<void()> guiRender) : guiRender(guiRender)
	{
	}
};

#endif /* GUICOMPONENT_H */
