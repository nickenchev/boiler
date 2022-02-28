#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "core/asset.h"

namespace Boiler
{

class Primitive : public Asset
{
    unsigned int vertexCount;
    unsigned int indexCount;

public:
	AssetId materialId;

    Primitive(const AssetId assetId, unsigned int vertexCount, unsigned int indexCount);

	unsigned int getVertexCount() const { return vertexCount; }
	unsigned int getIndexCount() const { return indexCount; }
};

}

#endif /* PRIMITIVE_H */
