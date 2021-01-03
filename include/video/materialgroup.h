#ifndef MATERIALGROUP_H
#define MATERIALGROUP_H

#include <vector>
#include "video/primitive.h"

namespace Boiler {

struct MaterialGroup
{
	struct PrimitiveInstance
	{
		const Primitive primitive;
		AssetId matrixId;

		PrimitiveInstance(const Primitive &primitive, AssetId matrixId) : primitive(primitive)
		{
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
