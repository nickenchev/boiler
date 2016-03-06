#ifndef RECT
#define RECT

#include <glm/glm.hpp>

#define DIM_X 0
#define DIM_Y 1

struct Size
{
    int dimensions[2];
    Size() { dimensions[0] = 0; dimensions[1] = 0; }
    Size(int width, int height) : dimensions {width, height} { }

    int getWidth() const { return dimensions[DIM_X]; }
    int getHeight() const { return dimensions[DIM_Y]; }
    void setWidth(int width) { dimensions[DIM_X] = width; }
    void setHeight(int height) { dimensions[DIM_Y] = height; }
};

struct Rect
{
    glm::vec3 position;
    Size size;
    glm::vec3 pivot;

    Rect() { }
    Rect(int x, int y, int width, int height) : position(x, y, 0), size(width, height), pivot(0, 0, 0) { }
    Rect(int x, int y, const Size &size) : position(x, y, 0), size(size), pivot(0, 0, 0) { }
    Rect(const Rect &rect) { *this = rect; }

    inline float getMin(int dimension) const
    {
        return position[dimension] - (size.dimensions[dimension] * pivot[dimension]);
    } 
    inline float getMax(int dimension) const
    {
        return position[dimension] + size.dimensions[dimension] - (size.dimensions[dimension] * pivot[dimension]);
    }

    bool collides(const Rect &rect) const;
    bool collides(const glm::vec2 point) const;
};

#endif // RECT

