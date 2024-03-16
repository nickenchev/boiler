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
	vec3 direction, up, panning;
	CameraType type;

	CameraComponent() : ComponentType(this)
	{
		direction = { 0, 0, 0 };
		up = { 0, 0, 0 };
		type = CameraType::firstPerson;
	}
};

}
