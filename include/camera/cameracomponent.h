#ifndef CAMERACOMPONENT_H
#define CAMERACOMPONENT_H

#include "core/componenttype.h"
#include "core/math.h"

namespace Boiler
{

struct CameraComponent : public ComponentType<CameraComponent>
{
	vec3 direction, up;

	CameraComponent() : ComponentType(this)
	{
		direction = {0, 0, 0};
		up = {0, 0, 0};
	}
};

}

#endif /* CAMERACOMPONENT_H */
