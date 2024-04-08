#ifndef ASSET_H
#define ASSET_H

#include <string>

namespace Boiler
{
	
using AssetId = int;

class Asset
{
public:

    virtual ~Asset() { }

	static constexpr int noAsset() { return -1; }
};


}

#endif /* ASSET_H */
