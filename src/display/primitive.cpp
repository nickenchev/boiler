#include "display/primitive.h"
#include "display/material.h"

using namespace Boiler;

Primitive::Primitive(AssetId bufferId, VertexData &&vertexData) : vertexData(vertexData)
{
	this->bufferId = bufferId;
	this->materialId = Asset::NO_ASSET;
}
