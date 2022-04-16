#ifndef MESH_H
#define MESH_H

#include <vector>
#include "display/primitive.h"

namespace Boiler
{
	
struct Mesh
{
	std::vector<Primitive> primitives;
};

}

#endif /* MESH_H */