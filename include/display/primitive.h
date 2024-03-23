#pragma once
#include "core/asset.h"
#include "display/vertexdata.h"

namespace Boiler
{

class Primitive : public Asset
{
	VertexData vertexData;

public:
	AssetId bufferId, materialId;
	vec3 min, max;

	Primitive();
    Primitive(AssetId bufferId, VertexData &&vertexData, vec3 min, vec3 max, AssetId materialId = Asset::noAsset());

	const VertexData &getVertexData() const { return vertexData; }
	auto indexCount() const { return vertexData.indexArray().size(); }
};

}
