#pragma once

#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include "core/componenttype.h"
#include "core/math.h"

namespace Boiler
{

class TransformComponent : public ComponentType<TransformComponent>
{
public:
	vec3 translation;
    vec3 scale;
	quat orientation;

	TransformComponent() : TransformComponent({0, 0, 0}) { }
	
	TransformComponent(vec3 translation) : ComponentType(this)
	{
		this->translation = translation;
		scale = { 1.0f, 1.0f, 1.0f };
		orientation = { 0, 0, 0, 0 };
	}

	// interface methods
	const mat4 getMatrix()
	{
		// create the model matrix, by getting a 3D vector from the Entity's vec2 position
		mat4 translationMatrix = translate(mat4(1), translation);
		mat4 rotationMatrix = glm::toMat4(glm::normalize(orientation));
		mat4 scaleMatrix = glm::scale(mat4(1), vec3(scale.x, scale.y, scale.z));

		return translationMatrix * rotationMatrix * scaleMatrix;
	}
};

}
