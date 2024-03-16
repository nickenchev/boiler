#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include <array>
#include <assert.h>

#include "core/asset.h"

namespace Boiler {

template<typename AssetType, std::size_t Size>
class AssetManager
{
	std::array<AssetType, Size> assets;
	std::array<bool, Size> assetStates;

public:
    AssetManager() : assetStates{false}
	{
	}

	AssetId add(AssetType &&asset)
	{
		AssetId assetId = Asset::noAsset();

		unsigned int index = 0;
		while (assetId == Asset::noAsset() && index < Size)
		{
			if (!assetStates[index])
			{
				assetId = index;
				assets[index] = std::move(asset); // TODO: This is probably wrong, need to move correctly
				assetStates[index] = true;
			}
			index++;
		}
		assert(assetId != Asset::noAsset());

		return assetId;
	}

	void remove(AssetId id)
	{
		assetStates[id] = false;
	}

	AssetType &operator()(AssetId assetId)
	{
		return get(assetId);
	}
	AssetType &get(AssetId assetId)
	{
		assert(assetStates[assetId] != false);
		return assets[assetId];
	}

	const AssetType &get(AssetId assetId) const
	{
		assert(assetStates[assetId] != false);
		return assets[assetId];
	}

	bool isOccupied(AssetId id) const
	{
		return assetStates[id];
	}

	AssetType &operator[](std::size_t index)
	{
		return assets[index];
	}

	void reset()
	{
		memset(assetStates.data(), 0, sizeof(bool) * Size);
	}

	const AssetType *data() const
	{
		return assets.data();
	}

	const auto &getAssets() const { return assets; }
	std::size_t getSize() const { return Size; }
	std::size_t byteSize() const { return Size * sizeof(AssetType); }
};

}

#endif /* ASSETMANAGER_H */
