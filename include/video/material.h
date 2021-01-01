#ifndef MATERIAL_H
#define MATERIAL_H

#include <optional>
#include "core/math.h"
#include "core/asset.h"
#include "video/texture.h"
#include "video/alphamode.h"

namespace Boiler {

struct Material : public Asset
{
    vec4 color;
	std::optional<Texture> baseTexture;
	AlphaMode alphaMode;
	
    Material(const AssetId assetId) : Asset(assetId)
	{
		color = vec4(1, 1, 1, 1);
	}
    virtual ~Material() { }
};

}

#endif /* MATERIAL_H */
