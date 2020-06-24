#ifndef MESH_H
#define MESH_H

#include <memory>
#include "video/model.h"
#include "core/spritesheetframe.h"

namespace Boiler
{
	
struct Mesh
{
	std::shared_ptr<const Model> model;
	const SpriteSheetFrame spriteSheetFrame;
    vec4 colour;

	Mesh(const std::shared_ptr<const Model> model, SpriteSheetFrame spriteSheetFrame) : spriteSheetFrame(spriteSheetFrame)
	{
		this->model = model;
		this->colour = vec4(1, 1, 1, 1);
	}
};

}

#endif /* MESH_H */
