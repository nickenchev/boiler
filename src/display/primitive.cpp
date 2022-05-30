#include "display/primitive.h"
#include "display/material.h"

using namespace Boiler;

Primitive::Primitive(AssetId bufferId, unsigned int vertexCount, unsigned int indexCount)
{
	this->bufferId = bufferId;
	this->vertexCount = vertexCount;
	this->indexCount = indexCount;
	this->materialId = Asset::NO_ASSET;
}
