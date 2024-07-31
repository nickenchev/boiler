#ifndef RENDERCOMPONENT_H
#define RENDERCOMPONENT_H

#include <vector>

#include "core/math.h"
#include "core/componenttype.h"
#include "display/mesh.h"

namespace Boiler
{

struct RenderComponent : public ComponentType<RenderComponent>
{
	AssetId meshId;
	bool hidden;

	RenderComponent(AssetId meshId = Asset::noAsset()) : ComponentType(this)
	{
		this->meshId = meshId;
		hidden = false;
	}
};

}
#endif /* RENDERCOMPONENT_H */
