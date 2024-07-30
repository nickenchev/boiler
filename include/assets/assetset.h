#ifndef ASSETSET_H
#define ASSETSET_H

#include <array>
#include "core/assetmanager.h"
#include "display/material.h"
#include "display/mesh.h"
#include "display/primitive.h"
#include "display/glyphmap.h"

namespace Boiler
{

struct AssetSet
{
	AssetManager<Material, 128> materials;
	AssetManager<Mesh, 2048> meshes;
	AssetManager<Primitive, 2048> primitives;
	AssetManager<GlyphMap, 2048> glyphs;
};

}

#endif /* ASSETSET_H */
