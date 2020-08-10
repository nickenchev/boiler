#ifndef MATERIAL_H
#define MATERIAL_H

#include <optional>
#include "core/math.h"
#include "video/texture.h"
#include "video/alphamode.h"

namespace Boiler {

struct Material
{
    vec4 color;
	std::optional<Texture> baseTexture;
	AlphaMode alphaMode;
	
    Material()
	{
		color = vec4(1, 1, 1, 1);
	}
    virtual ~Material() { }
};

}

#endif /* MATERIAL_H */
