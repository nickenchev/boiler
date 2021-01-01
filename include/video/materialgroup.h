#ifndef MATERIALGROUP_H
#define MATERIALGROUP_H

#include <vector>
#include "video/primitive.h"

namespace Boiler {

struct MaterialGroup
{
	AssetId materialId;
	AssetId matrixId;
	std::vector<Primitive> primitives;

	MaterialGroup()
	{
		materialId = Asset::NO_ASSET;
		matrixId = Asset::NO_ASSET;
	}
};

}

#endif /* MATERIALGROUP_H */
