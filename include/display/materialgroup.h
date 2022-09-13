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
		vec3 drawOffset;

		PrimitiveInstance(AssetId primitiveId, AssetId matrixId) : PrimitiveInstance(primitiveId, matrixId, vec3(0)) { }
		PrimitiveInstance(AssetId primitiveId, AssetId matrixId, vec3 drawOffset) : drawOffset(drawOffset)
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
