#ifndef COMPONENTTYPE_H
#define COMPONENTTYPE_H

#include <iostream>
#include "component.h"

namespace Boiler
{

template <typename T>
class ComponentType : public Component
{
	const T *component;
public:
	static int storageIndex;
	static ComponentMask mask;

	ComponentType(const T *component) : component(component)
	{
	}

	const ComponentMask &getMask() const override { return T::mask; }

	Component *clone() override
	{
		return new T(*component);
	}
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

template <typename T>
ComponentMask ComponentType<T>::mask{componentMask()};

inline int getStorageIndex()
{
	static int index = 0;
	return index++;
}

template <typename T>
int ComponentType<T>::storageIndex = getStorageIndex();

}

#endif /* COMPONENTTYPE_H */
