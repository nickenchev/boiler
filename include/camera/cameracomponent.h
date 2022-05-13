#ifndef CAMERACOMPONENT_H
#define CAMERACOMPONENT_H

#include "core/componenttype.h"
#include "core/math.h"

namespace Boiler
{

class CameraComponent : public ComponentType<CameraComponent>
{
public:
	CameraComponent() : ComponentType(this)
	{
	}
};

}

#endif /* CAMERACOMPONENT_H */
