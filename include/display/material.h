#ifndef MATERIAL_H
#define MATERIAL_H

#include <optional>
#include "core/math.h"
#include "core/asset.h"
#include "display/texture.h"
#include "display/alphamode.h"
#include "display/imaging/imagedata.h"

namespace Boiler {

class Material : public Asset
{
	bool systemRelated;
public:
	std::string name;
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
    float shininess;

	AssetId albedoTexture;
	ImageData albedoData;
	AssetId normalTexture;
	ImageData normalData;
	AssetId metallicRougnessTexture;
	ImageData metallicRoughnessData;
	AlphaMode alphaMode;
	bool depth;
	
    Material(bool systemRelated = false) : systemRelated(systemRelated)
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

	Material &operator=(Material &&mat)
	{
		name = mat.name;
		ambient = mat.ambient;
		diffuse = mat.diffuse;
		specular = mat.specular;
		shininess = mat.shininess;
		albedoTexture = mat.albedoTexture;
		albedoData = std::move(mat.albedoData);
		normalTexture = mat.normalTexture;
		normalData = std::move(mat.normalData);
		metallicRougnessTexture = mat.metallicRougnessTexture;
		metallicRoughnessData = std::move(mat.metallicRoughnessData);
		alphaMode = mat.alphaMode;
		depth = mat.depth;

		return *this;
	}

    virtual ~Material() { }
	bool isSystemRelated() const { return systemRelated; }
};

}

#endif /* MATERIAL_H */
