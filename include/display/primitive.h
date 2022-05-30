#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include "core/asset.h"
#include "display/vertexdata.h"

namespace Boiler
{

class Primitive : public Asset
{
	VertexData vertexData;

public:
	AssetId bufferId = Asset::NO_ASSET;
	AssetId materialId = Asset::NO_ASSET;

	Primitive() { }
    Primitive(AssetId bufferId, VertexData &&vertexData);

	VertexData &getVertexData() { return vertexData; }
	auto indexCount() const { return vertexData.indexArray().size(); }
};

}

#endif /* PRIMITIVE_H */
