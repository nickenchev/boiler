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
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;

	std::optional<Texture> baseTexture;
	AlphaMode alphaMode;
	bool depth;
	
    Material(const AssetId assetId) : Asset(assetId)
	{
		depth = true;
		ambient = vec4(1, 1, 1, 1);
		diffuse = vec4(1, 1, 1, 1);
		specular = vec4(1, 1, 1, 1);
		shininess = 0;
		alphaMode = AlphaMode::BLEND;
	}
    virtual ~Material() { }
};

}

#endif /* MATERIAL_H */
