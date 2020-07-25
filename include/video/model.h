#ifndef MODEL_H
#define MODEL_H

#include "core/asset.h"

namespace Boiler
{

class Model : public Asset
{
    const unsigned int vertexCount;
    const unsigned int indexCount;

public:
    Model(const AssetId assetId, unsigned int vertexCount, unsigned int indexCount);

	unsigned int getVertexCount() const { return vertexCount; }
	unsigned int getIndexCount() const { return indexCount; }
};

}
#endif /* MODEL_H */
