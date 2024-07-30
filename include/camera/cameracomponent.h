#pragma once

#include "core/componenttype.h"
#include "core/math.h"

namespace Boiler
{

	enum class CameraType
	{
		firstPerson,
		arcball
	};

struct CameraComponent : public ComponentType<CameraComponent>
{
	vec3 offset, up;
	CameraType type;

	CameraComponent() : ComponentType(this)
	{
		up = { 0, 1, 0 };
		type = CameraType::firstPerson;
	}
};

}
