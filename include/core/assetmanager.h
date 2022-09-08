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
		reset();
	}

	AssetId add(AssetType &&asset)
	{
		size++;
		AssetId assetId = index++;
		assets[assetId] = std::move(asset);

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

	void reset()
	{
		index = 0;
		size = 0;
	}

	const AssetType *data() const
	{
		return assets.data();
	}

	const auto &getAssets() const { return assets; }
	std::size_t getSize() const { return size; }
	std::size_t byteSize() const { return getSize() * sizeof(AssetType); }
};

}

#endif /* ASSETMANAGER_H */
