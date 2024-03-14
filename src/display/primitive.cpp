#include "display/primitive.h"
#include "display/material.h"

using namespace Boiler;

Primitive::Primitive()
{
	bufferId = Asset::noAsset();
	materialId = Asset::noAsset();
	min = vec3(0, 0, 0);
	max = vec3(0, 0, 0);
}

Primitive::Primitive(AssetId bufferId, VertexData &&vertexData, vec3 min, vec3 max) : vertexData(vertexData)
{
	this->bufferId = bufferId;
	this->materialId = Asset::noAsset();
	this->min = min;
	this->max = max;
}
