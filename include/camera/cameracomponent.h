#ifndef CAMERACOMPONENT_H
#define CAMERACOMPONENT_H

#include "core/componenttype.h"
#include "core/math.h"

namespace Boiler
{

class CameraComponent : public ComponentType<CameraComponent>
{
public:
	vec3 direction, up;
	
	CameraComponent() : ComponentType(this)
	{
		direction = {0, 0, 0};
	}
};

}

#endif /* CAMERACOMPONENT_H */
