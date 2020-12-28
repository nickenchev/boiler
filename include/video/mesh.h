#ifndef MESH_H
#define MESH_H

#include <vector>
#include "video/primitive.h"

namespace Boiler
{
	
struct Mesh
{
	std::vector<Primitive> primitives;
};

}

#endif /* MESH_H */
