#ifndef POSITIONCOMPONENT_H
#define POSITIONCOMPONENT_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "core/componenttype.h"
#include "core/rect.h"

namespace Boiler
{

struct PositionComponent : public ComponentType<PositionComponent>
{
    Rect frame;
    bool flipH, flipV, collides;
    glm::vec3 scale;
    bool absolute;

	PositionComponent(Rect frame) : frame(frame), scale(1.0f, 1.0f, 1.0f)
	{
		flipH = false;
		flipV = false;
		collides = false;
		absolute = false;
	}

	const glm::mat4 getMatrix() const
	{
		// offset the player position based on the pivot modifier
		glm::vec3 pivotPos(frame.position.x - frame.size.width * frame.pivot.x,
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
		glm::mat4 modelMatrix;
		modelMatrix = glm::translate(glm::mat4(1), pivotPos);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(scaleX, scaleY, 1.0f));

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
