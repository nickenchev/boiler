#ifndef ASSETSET_H
#define ASSETSET_H

#include <array>
#include "core/assetmanager.h"
#include "display/material.h"
#include "display/primitive.h"

namespace Boiler
{

struct AssetSet
{
	static const unsigned int MaxAssets = 512;
	
	AssetManager<Material, MaxAssets> materials;
	AssetManager<Primitive, MaxAssets> primitives;
};

}

#endif /* ASSETSET_H */
