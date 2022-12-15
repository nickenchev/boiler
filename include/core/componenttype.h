#ifndef COMPONENTTYPE_H
#define COMPONENTTYPE_H

#include "component.h"

namespace Boiler
{

template <typename T>
class ComponentType : public Component
{
	const T *component;

public:
	static unsigned int storageIndex;
	static ComponentMask mask;

	ComponentType(const T *component) : component(component)
	{
	}

	const ComponentMask &getMask() const override { return T::mask; }
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

inline unsigned int getStorageIndex()
{
	static unsigned int index = 0;
	static bool firstIndex = true;
	if (!firstIndex)
	{
		index++;
	}
	else
	{
		firstIndex = false;
	}
	return index;
}

template <typename T>
unsigned int ComponentType<T>::storageIndex{getStorageIndex()};

}

#endif /* COMPONENTTYPE_H */
