#ifndef RENDERCOMPONENT_H
#define RENDERCOMPONENT_H

#include <vector>

#include "core/math.h"
#include "core/componenttype.h"
#include "video/mesh.h"

namespace Boiler
{

struct RenderComponent : public ComponentType<RenderComponent>
{
	Mesh mesh;
};

}
#endif /* RENDERCOMPONENT_H */
