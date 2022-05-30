#ifndef ASSET_H
#define ASSET_H

namespace Boiler
{
	
using AssetId = int;

class Asset
{
public:
	static constexpr int NO_ASSET = -1;

	Asset() {}
    virtual ~Asset() { }
};


}

#endif /* ASSET_H */
