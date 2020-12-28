#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include <array>
#include "core/asset.h"

namespace Boiler {

template<typename AssetType, std::size_t Size>
class AssetManager
{
	size_t size, index;
	std::array<AssetType, Size> assets;

public:
    AssetManager()
	{
		index = 0;
	}

	AssetId add(AssetType asset)
	{
		size++;
		AssetId assetId = index++;
		assets[assetId] = asset;

		return assetId;
	}

	AssetType &getAsset(AssetId assetId)
	{
		return assets[assetId];
	}

	auto & getAssets() const { return assets; }
};

}

#endif /* ASSETMANAGER_H */
