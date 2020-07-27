#include "video/primitive.h"

using namespace Boiler;

Primitive::Primitive(AssetId assetId, unsigned int vertexCount, unsigned int indexCount)
	: Asset(assetId), vertexCount(vertexCount), indexCount(indexCount)
{
}
