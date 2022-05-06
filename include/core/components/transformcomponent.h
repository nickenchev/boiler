#ifndef TRANSFORMCOMPONENT_H
#define TRANSFORMCOMPONENT_H

#include <glm/gtc/matrix_transform.hpp>
#include "core/componenttype.h"
#include "core/rect.h"
#include "core/math.h"

namespace Boiler
{

class TransformComponent : public ComponentType<TransformComponent>
{
    Rect frame;
	vec3 velocity;
	vec3 acceleration;
    vec3 scale;
	quat orientation;
    bool absolute;

	mat4 matrix;
	bool dirty;

public:
	TransformComponent() : TransformComponent(Rect(0, 0, 0, 0)) { }
	
	TransformComponent(Rect frame) : ComponentType(this), frame(frame)
	{
		velocity = {0, 0, 0};
		acceleration = {0, 0, 0};
		scale = {1.0f, 1.0f, 1.0f};
		orientation = {0, 0, 0, 0};
		absolute = false;
		dirty = true;
		matrix = mat4();
	}

	// interface methods
	const mat4 getMatrix()
	{
		if (dirty)
		{
			// offset the player position based on the pivot modifier
			vec3 pivotPos(frame.position.x - frame.size.width * frame.pivot.x,
						frame.position.y - frame.size.height * frame.pivot.y,
						frame.position.z);

			// create the model matrix, by getting a 3D vector from the Entity's vec2 position
			mat4 scaleMatrix = glm::scale(mat4(1), vec3(scale.x, scale.y, scale.z));
			mat4 rotationMatrix = glm::toMat4(glm::normalize(orientation));
			mat4 translationMatrix = translate(mat4(1), pivotPos);

			matrix = translationMatrix * rotationMatrix * scaleMatrix;
			dirty = false;
		}
		return matrix;
	}

	bool isDirty() const { return dirty; }

	vec3 getPosition() const { return frame.position; }
	void setPosition(float x, float y, float z) { setPosition(vec3(x, y, z)); }
	void setPosition(const vec3 &position)
	{
		frame.position = position;
		dirty = true;
	}

	vec3 getScale() const { return scale; }
	void setScale(float value)
	{
		setScale(vec3(value, value, value));
	}
	void setScale(float x, float y, float z)
	{
		setScale(vec3(x, y, z));
	}
	void setScale(const vec3 &scale)
	{
		this->scale = scale;
		dirty = true;
	}

	quat getOrientation() const { return orientation; }
	void setOrientation(quat orientation)
	{
		this->orientation = orientation;
		dirty = true;
	}

	vec3 getVelocity() const { return velocity; }
	void setVelocity(vec3 velocity)
	{
		this->velocity = velocity;
	}

	vec3 getAcceleration() const { return acceleration; }
	void setAcceleration(vec3 acceleration)
	{
		this->acceleration = acceleration;
	}

	// operators
	TransformComponent operator+(const TransformComponent &comp2) const
	{
		TransformComponent comp = *this;
		comp.frame.position += comp2.frame.position;
		return comp;
	}

};
}

#endif /* TRANSFORMCOMPONENT_H */
