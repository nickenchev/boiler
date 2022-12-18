#ifndef MATERIALGROUP_H
#define MATERIALGROUP_H

#include <vector>
#include "core/asset.h"
#include "core/rect.h"

namespace Boiler {

struct MaterialGroup
{
	struct PrimitiveInstance
	{
		AssetId primitiveId, matrixId;
		vec3 drawOffset;
		uint32_t indexCount;
		uint32_t vertexOffset;
		uint32_t indexOffset;
		uint32_t vertexBufferStart;
		uint32_t indexBufferStart;
		Rect clip;
		bool shouldClip;

		PrimitiveInstance(AssetId primitiveId, AssetId matrixId) : PrimitiveInstance(primitiveId, matrixId, vec3(0), 0, 0, 0) { }

		PrimitiveInstance(AssetId primitiveId, AssetId matrixId, vec3 drawOffset) : PrimitiveInstance(primitiveId, matrixId, drawOffset, 0, 0, 0) { }

		PrimitiveInstance(AssetId primitiveId, AssetId matrixId, vec3 drawOffset, uint32_t indexCount, uint32_t vertexOffset, uint32_t indexOffset, uint32_t vertexBufferStart = 0, uint32_t indexBufferStart = 0)
		{
			this->primitiveId = primitiveId;
			this->matrixId = matrixId;
			this->drawOffset = drawOffset;
			this->indexCount = indexCount;
			this->vertexOffset = vertexOffset;
			this->indexOffset = indexOffset;
			this->vertexBufferStart = vertexBufferStart;
			this->indexBufferStart = indexBufferStart;
			this->shouldClip = false;
		}
	};
	
	AssetId materialId;
	std::vector<PrimitiveInstance> primitives; // TODO: Shouldn't be using a vector, dynamic alloc sucks mid frame

	MaterialGroup()
	{
		materialId = Asset::NO_ASSET;
	}
	MaterialGroup(AssetId materialId)
	{
		this->materialId = materialId;
	}
};

}

#endif /* MATERIALGROUP_H */
