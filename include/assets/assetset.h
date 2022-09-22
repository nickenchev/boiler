#ifndef ASSETSET_H
#define ASSETSET_H

#include <array>
#include "core/assetmanager.h"
#include "display/material.h"
#include "display/primitive.h"
#include "display/glyphmap.h"

namespace Boiler
{

struct AssetSet
{
	static const unsigned int MaxAssets = 2048;
	
	AssetManager<Material, MaxAssets> materials;
	AssetManager<Primitive, MaxAssets> primitives;
	AssetManager<GlyphMap, MaxAssets> glyphs;
};

}

#endif /* ASSETSET_H */
