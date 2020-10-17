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

	bool operator==(const AssetId &assetId)
	{
		return getAssetId() == assetId;
	}

	AssetId getAssetId() const { return assetId; }
};


}

#endif /* ASSET_H */
