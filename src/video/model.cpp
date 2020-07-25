#include "video/model.h"

using namespace Boiler;

Model::Model(AssetId assetId, unsigned int vertexCount, unsigned int indexCount)
	: Asset(assetId), vertexCount(vertexCount), indexCount(indexCount)
{
}
