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
	Mesh mesh;

	RenderComponent() : ComponentType(this) { }
};

}
#endif /* RENDERCOMPONENT_H */
