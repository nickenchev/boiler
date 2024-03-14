#ifndef MATERIAL_H
#define MATERIAL_H

#include <optional>
#include "core/math.h"
#include "core/asset.h"
#include "display/texture.h"
#include "display/alphamode.h"

namespace Boiler {

struct Material : public Asset
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;

	AssetId albedoTexture;
	AssetId normalTexture;
	AssetId metallicRougnessTexture;
	AlphaMode alphaMode;
	bool depth;
	
    Material()
	{
		depth = true;
		ambient = vec4(1, 1, 1, 1);
		diffuse = vec4(1, 1, 1, 1);
		specular = vec4(1, 1, 1, 1);
		shininess = 0;
		alphaMode = AlphaMode::opaque;
		albedoTexture = Asset::noAsset();
		normalTexture = Asset::noAsset();
		metallicRougnessTexture = Asset::noAsset();
	}
    virtual ~Material() { }
};

}

#endif /* MATERIAL_H */
