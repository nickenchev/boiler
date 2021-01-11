#ifndef SIZE_H
#define SIZE_H

#include "core/common.h"

namespace Boiler {

struct Size
{
	cgfloat width, height;

	Size() { width = 0; height = 0; }
	Size(cgfloat width, cgfloat height) : width(width), height(height) { }
};

}
#endif /* SIZE_H */
