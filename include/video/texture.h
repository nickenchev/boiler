#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include "core/asset.h"

namespace Boiler
{

class Texture : public Asset
{
    std::string filePath;

public:
    Texture(const AssetId assetId, const std::string filePath) : Asset(assetId), filePath(filePath) { }

    const std::string &getFilePath() const { return filePath; }
};

}

#endif /* TEXTURE_H */
