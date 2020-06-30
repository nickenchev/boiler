#ifndef POSITIONCOMPONENT_H
#define POSITIONCOMPONENT_H

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include "core/componenttype.h"
#include "core/rect.h"
#include "core/math.h"

namespace Boiler
{

struct PositionComponent : public ComponentType<PositionComponent>
{
    Rect frame;
    vec3 scale;
	glm::quat orientation;
    bool absolute;

	PositionComponent(Rect frame) : frame(frame)
	{
		scale = {1.0f, 1.0f, 1.0f};
		orientation = {0, 0, 0, 0};
		absolute = false;
	}

	const mat4 getMatrix() const
	{
		// offset the player position based on the pivot modifier
		vec3 pivotPos(frame.position.x - frame.size.width * frame.pivot.x,
					  frame.position.y - frame.size.height * frame.pivot.y,
					  frame.position.z);

		// create the model matrix, by getting a 3D vector from the Entity's vec2 position
		mat4 scaleMatrix = glm::scale(mat4(1), vec3(scale.x, scale.y, scale.z));
		mat4 rotationMatrix = glm::mat4(orientation);
		mat4 translationMatrix = translate(mat4(1), pivotPos);
		mat4 modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;

		return modelMatrix;
	}

	PositionComponent operator+(const PositionComponent &comp2) const
	{
		PositionComponent comp = *this;
		comp.frame.position += comp2.frame.position;
		return comp;
	}
};

}

#endif /* POSITIONCOMPONENT_H */
