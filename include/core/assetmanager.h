#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include <array>
#include "core/asset.h"

namespace Boiler {

template<typename AssetType, std::size_t Size>
class AssetManager
{
	std::size_t size, index;
	std::array<AssetType, Size> assets;

public:
    AssetManager()
	{
		index = 0;
	}

	AssetId add(AssetType &&asset)
	{
		size++;
		AssetId assetId = index++;
		assets[assetId] = asset;

		return assetId;
	}

	AssetType &get(AssetId assetId)
	{
		return assets[assetId];
	}

	AssetType &operator[](std::size_t index)
	{
		return assets[index];
	}

	const auto &getAssets() const { return assets; }
	std::size_t getSize() const { return size; }
};

}

#endif /* ASSETMANAGER_H */
