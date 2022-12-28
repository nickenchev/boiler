#ifndef SIZE_H
#define SIZE_H

#include "core/common.h"

namespace Boiler {

struct Size
{
	cgfloat width, height;

	Size() { width = 0; height = 0; }
	Size(cgfloat width, cgfloat height) : width(width), height(height) { }

	bool operator==(const Size &size)
	{
		return size.width == width && size.height == height;
	}

	bool operator!=(const Size &size)
	{
		return size.width != width && size.height != height;
	}
};

}
#endif /* SIZE_H */
