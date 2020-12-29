#ifndef MATERIALGROUP_H
#define MATERIALGROUP_H

#include <vector>
#include "core/asset.h"

namespace Boiler {

struct MaterialGroup
{
	AssetId matrixId;
	std::vector<AssetId> primitives;
};

}

#endif /* MATERIALGROUP_H */
