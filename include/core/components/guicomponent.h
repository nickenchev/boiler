#ifndef GUICOMPONENT_H
#define GUICOMPONENT_H

#include <functional>
#include "core/componenttype.h"

namespace Boiler
{

struct GUIComponent : public ComponentType<GUIComponent>
{
	const std::function<void()> guiRender;
	
	GUIComponent(std::function<void()> guiRender) : guiRender(guiRender), ComponentType(this)
	{
	}
};

}

#endif /* GUICOMPONENT_H */
