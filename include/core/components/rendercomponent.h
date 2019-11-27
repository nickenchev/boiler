#ifndef RENDERCOMPONENT_H
#define RENDERCOMPONENT_H

#include <memory>
#include "core/math.h"
#include "core/componenttype.h"
#include "video/model.h"
#include "core/spritesheetframe.h"

namespace Boiler
{

struct RenderComponent : public ComponentType<RenderComponent>
{
	std::shared_ptr<const Model> model;
	const SpriteSheetFrame spriteSheetFrame;
    vec4 colour;

	RenderComponent(const std::shared_ptr<const Model> model, SpriteSheetFrame spriteSheetFrame) : spriteSheetFrame(spriteSheetFrame)
	{
		this->model = model;
		this->colour = vec4(1, 1, 1, 1);
	}
};

}
#endif /* RENDERCOMPONENT_H */
