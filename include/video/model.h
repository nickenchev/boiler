#ifndef MODEL_H
#define MODEL_H

#include "core/asset.h"

namespace Boiler
{

class Model
{
	const AssetId assetId;
    const unsigned int vertexCount;
    const unsigned int indexCount;

public:
    Model(AssetId assetId, unsigned int vertexCount, unsigned int indexCount);

	AssetId getAssetId() const { return assetId; }
	unsigned int getVertexCount() const { return vertexCount; }
	unsigned int getIndexCount() const { return indexCount; }
};

}
#endif /* MODEL_H */
