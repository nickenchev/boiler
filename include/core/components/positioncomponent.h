#ifndef POSITIONCOMPONENT_H
#define POSITIONCOMPONENT_H

#include <glm/gtc/matrix_transform.hpp>
#include "core/componenttype.h"
#include "core/rect.h"
#include "core/math.h"

namespace Boiler
{

struct PositionComponent : public ComponentType<PositionComponent>
{
    Rect frame;
	vec3 rotationAxis;
	float rotationAngle;
    bool flipH, flipV, collides;
    vec3 scale;
    bool absolute;

	PositionComponent(Rect frame) : frame(frame), scale(1.0f, 1.0f, 1.0f), rotationAxis(0, 0, 0)
	{
		flipH = false;
		flipV = false;
		collides = false;
		absolute = false;
		rotationAngle = 0;
	}

	const mat4 getMatrix() const
	{
		// offset the player position based on the pivot modifier
		vec3 pivotPos(frame.position.x - frame.size.width * frame.pivot.x,
						   frame.position.y - frame.size.height * frame.pivot.y, 0);

		float scaleX = scale.x;
		float scaleY = scale.y;
		if (flipH)
		{
			pivotPos.x += frame.size.width;
			scaleX *= -1;
		}
		if (flipV)
		{
			pivotPos.x += frame.size.height;
			scaleY *= -1;
		}
		// create the model matrix, by getting a 3D vector from the Entity's vec2 position
		mat4 modelMatrix;
		modelMatrix = translate(mat4(1), pivotPos);
		modelMatrix = glm::scale(modelMatrix, vec3(scaleX, scaleY, 1.0f));
		modelMatrix = glm::rotate(modelMatrix, glm::radians(rotationAngle), rotationAxis);

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
