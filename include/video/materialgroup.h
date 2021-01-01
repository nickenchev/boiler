#ifndef MATERIALGROUP_H
#define MATERIALGROUP_H

#include <vector>
#include "video/primitive.h"

namespace Boiler {

struct MaterialGroup
{
	AssetId matrixId;
	std::vector<Primitive> primitives;
};

}

#endif /* MATERIALGROUP_H */
