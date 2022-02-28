#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include "core/asset.h"

namespace Boiler
{

class Texture : public Asset
{
public:
    Texture(const AssetId assetId) : Asset(assetId) { }
};

}

#endif /* TEXTURE_H */
