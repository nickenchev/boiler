#ifndef MESH_H
#define MESH_H

#include <vector>
#include "core/asset.h"
#include "core/math.h"

namespace Boiler
{
	
struct Mesh
{
	std::vector<AssetId> primitives;
	vec3 min, max;
};

}

#endif /* MESH_H */
