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
	AssetId bufferId = Asset::NO_ASSET;
	AssetId materialId = Asset::NO_ASSET;

	Primitive() { }
    Primitive(AssetId bufferId, unsigned int vertexCount, unsigned int indexCount);

	unsigned int getVertexCount() const { return vertexCount; }
	unsigned int getIndexCount() const { return indexCount; }
};

}

#endif /* PRIMITIVE_H */
