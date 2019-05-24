#ifndef RECT
#define RECT

#include <glm/glm.hpp>

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
    glm::vec3 position;
    Size size;
    glm::vec3 pivot;

    Rect() { }
    Rect(cgfloat x, cgfloat y, cgfloat width, cgfloat height) : position(x, y, 0), size(width, height), pivot(0, 0, 0) { }
    Rect(cgfloat x, cgfloat y, const Size &size) : position(x, y, 0), size(size), pivot(0, 0, 0) { }
    Rect(const Rect &rect) { *this = rect; }

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

	inline glm::vec3 center() const
	{
		return glm::vec3(position.x + size.width / 2,
						 position.y + size.height / 2, 0);
	}

    bool collides(const Rect &rect) const;
    bool collides(const glm::vec2 point) const;
};

}

#endif // RECT
