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
    glm::vec2 position;
    glm::vec2 pivot;
    Size size;

    Rect() { }
    Rect(int x, int y, int width, int height) : position(x, y), size(width, height), pivot(0, 0) { }
    Rect(const Rect &rect) { *this = rect; }

    inline float getMin(int dimension) const
    {
        return position[dimension] - (size.dimensions[dimension] * pivot[dimension]);
    } 
    inline float getMax(int dimension) const
    {
        return position[dimension] + size.dimensions[dimension] - (size.dimensions[dimension] * pivot[dimension]);
    }

    bool collides(const Rect &rect);
};

#endif // RECT
