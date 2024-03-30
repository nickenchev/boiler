#ifndef ASSET_H
#define ASSET_H

#include <string>

namespace Boiler
{
	
using AssetId = int;

class Asset
{
	AssetId assetId;
public:

	Asset() : assetId(noAsset())
	{
	}
    virtual ~Asset() { }

	static constexpr int noAsset() { return -1; }
};


}

#endif /* ASSET_H */
