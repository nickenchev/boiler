#include "video/primitive.h"

using namespace Boiler;

Primitive::Primitive(AssetId assetId, unsigned int vertexCount, unsigned int indexCount)
	: Asset(assetId)
{
	this->vertexCount = vertexCount;
	this->indexCount = indexCount;
	this->materialId = 0;
}
