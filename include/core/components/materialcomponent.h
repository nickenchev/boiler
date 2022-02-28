#ifndef MATERIALCOMPONENT_H
#define MATERIALCOMPONENT_H

#include "core/componenttype.h"
#include "display/material.h"

namespace Boiler
{

struct MaterialComponent : public ComponentType<MaterialComponent>
{
	Material material;
};

}

#endif /* MATERIALCOMPONENT_H */
