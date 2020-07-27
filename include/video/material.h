#ifndef MATERIAL_H
#define MATERIAL_H

#include <optional>
#include "core/math.h"
#include "video/texture.h"

namespace Boiler {

struct Material
{
    vec4 color;
	std::optional<Texture> baseTexture;
	
    Material()
	{
		color = vec4(1, 1, 1, 1);
	}
    virtual ~Material() { }
};

}

#endif /* MATERIAL_H */
