#ifndef ASSET_H
#define ASSET_H

namespace Boiler
{
	
using AssetId = unsigned int;

class Asset
{
	AssetId assetId;
	
public:
    Asset(const AssetId assetId) : assetId(assetId) {}
    virtual ~Asset() { }

	AssetId getAssetId() const { return assetId; }
};


}

#endif /* ASSET_H */
