#ifndef MATERIALGROUP_H
#define MATERIALGROUP_H

#include <vector>
#include "display/primitive.h"

namespace Boiler {

struct MaterialGroup
{
	struct PrimitiveInstance
	{
		AssetId primitiveId, matrixId;

		PrimitiveInstance(AssetId primitiveId, AssetId matrixId)
		{
			this->primitiveId = primitiveId;
			this->matrixId = matrixId;
		}
	};
	
	AssetId materialId;
	std::vector<PrimitiveInstance> primitives;

	MaterialGroup()
	{
		materialId = Asset::NO_ASSET;
	}
};

}

#endif /* MATERIALGROUP_H */
