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
		vec3 offset;

		PrimitiveInstance(AssetId primitiveId, AssetId matrixId) : offset(0)
		{
			this->primitiveId = primitiveId;
			this->matrixId = matrixId;
		}
		PrimitiveInstance(AssetId primitiveId, AssetId matrixId, vec3 offset) : offset(offset)
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
