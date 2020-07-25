#ifndef MATERIAL_H
#define MATERIAL_H

#include <optional>
#include "video/texture.h"

namespace Boiler {

struct Material
{
	std::optional<Texture> baseTexture;
	
    Material() { }
    virtual ~Material() { }
};

}

#endif /* MATERIAL_H */
