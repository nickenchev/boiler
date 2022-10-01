#include "display/primitive.h"
#include "display/material.h"

using namespace Boiler;

Primitive::Primitive(AssetId bufferId, VertexData &&vertexData, vec3 min, vec3 max) : vertexData(vertexData)
{
	this->bufferId = bufferId;
	this->materialId = Asset::NO_ASSET;
	this->min = min;
	this->max = max;
}
