#ifndef MESH_H
#define MESH_H

#include <memory>
#include "video/model.h"
#include "core/spriteframe.h"

namespace Boiler
{
	
struct Mesh
{
	const Model model;
	SpriteFrame spriteFrame; // TODO: This doesn't belong here
    vec4 colour;

	Mesh(const Model &model, SpriteFrame spriteFrame) : model(model), spriteFrame(spriteFrame)
	{
		this->colour = vec4(1, 1, 1, 1);
	}
};

}

#endif /* MESH_H */
