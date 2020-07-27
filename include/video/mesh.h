#ifndef MESH_H
#define MESH_H

#include <vector>
#include "video/primitive.h"
#include "core/spriteframe.h"

namespace Boiler
{
	
struct Mesh
{
	std::vector<Primitive> primitives;
};

}

#endif /* MESH_H */
