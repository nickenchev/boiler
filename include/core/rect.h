#ifndef RECT
#define RECT

#include "core/math.h"

namespace Boiler
{

using cgfloat = float;

struct Size
{
	cgfloat width, height;

	Size() { width = 0; height = 0; }
	Size(cgfloat width, cgfloat height) : width(width), height(height) { }
};

struct Rect
{
    vec3 position;
    Size size;
    vec3 pivot;

    Rect() { }
    Rect(cgfloat x, cgfloat y, cgfloat width, cgfloat height) : position(x, y, 0), size(width, height), pivot(0, 0, 0) { }
    Rect(cgfloat x, cgfloat y, const Size &size) : position(x, y, 0), size(size), pivot(0, 0, 0) { }
    Rect(const Rect &rect) { *this = rect; }

	void operator=(const Rect &rect)
	{
		this->position = rect.position;
		this->size = rect.size;
		this->pivot = rect.pivot;
	}

    inline cgfloat getMinX() const
    {
		return position.x;
    } 
    inline cgfloat getMinY() const
    {
		return position.y;
    } 
    inline cgfloat getMaxX() const
    {
		return position.x + size.width;
    }
    inline cgfloat getMaxY() const
    {
		return position.y + size.height;
    }

	inline vec3 center() const
	{
		return vec3(position.x + size.width / 2,
					position.y + size.height / 2, 0);
	}

	bool collides(const Rect &rect, const vec3 &epsilon = vec3(0, 0, 0)) const;
    bool collides(const vec2 point) const;
};

}

#endif // RECT
