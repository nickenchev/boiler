#ifndef COMPONENTTYPE_H
#define COMPONENTTYPE_H

#include <iostream>
#include "component.h"

template <typename T>
class ComponentType : public Component
{
public:
	static ComponentMask mask;
	const ComponentMask &getMask() const { return T::mask; }
};

inline ComponentMask componentMask()
{
	static ComponentMask mask{1};
	ComponentMask newMask = mask;

	static bool firstMask = true;
	if (!firstMask)
	{
		mask <<= 1;
		newMask = mask;
	}
	else
	{
		firstMask = false;
	}
	return newMask;
}

template <typename T> ComponentMask ComponentType<T>::mask{componentMask()};

#endif /* COMPONENTTYPE_H */
