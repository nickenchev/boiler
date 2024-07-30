#ifndef MESH_H
#define MESH_H

#include <vector>
#include "core/asset.h"
#include "core/math.h"

namespace Boiler
{
	
struct Mesh
{
	std::string name;
	std::vector<AssetId> primitives;
	vec3 min, max;

	Mesh(AssetId primitiveId) : Mesh()
	{
		primitives.push_back(primitiveId);
	}

	Mesh()
	{
		min = {0, 0, 0};
		max = {0, 0, 0};
	}
};

}

#endif /* MESH_H */
