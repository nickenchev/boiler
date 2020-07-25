#ifndef MESH_H
#define MESH_H

#include <memory>
#include "video/model.h"
#include "core/spritesheetframe.h"

namespace Boiler
{
	
struct Mesh
{
	const Model model;
	SpriteSheetFrame spriteSheetFrame; // TODO: This doesn't belong here
    vec4 colour;

	Mesh(const Model &model, SpriteSheetFrame spriteSheetFrame) : model(model), spriteSheetFrame(spriteSheetFrame)
	{
		this->colour = vec4(1, 1, 1, 1);
	}
};

}

#endif /* MESH_H */
