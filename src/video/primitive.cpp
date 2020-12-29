#include "video/primitive.h"
#include "video/material.h"

using namespace Boiler;

Primitive::Primitive(AssetId assetId, unsigned int vertexCount, unsigned int indexCount)
	: Asset(assetId)
{
	this->vertexCount = vertexCount;
	this->indexCount = indexCount;
	this->materialId = Material::NO_MATERIAL;
}
