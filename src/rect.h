#ifndef RECT
#define RECT

#include <glm/glm.hpp>

#define DIM_X 0
#define DIM_Y 1

struct Size
{
    int width, height;
    Size() { width = 0; height = 0; }
    Size(int width, int height) : width(width), height(height) { }
};

struct Rect
{
    glm::vec2 position;
    Size size;
    Rect() { }
    Rect(int x, int y, int width, int height) : position(x, y), size(width, height) { }
    Rect(const Rect &rect) { *this = rect; }

    inline float getMin(int dimension) const
    {
        float result = 0;
        if (dimension == DIM_X)
        {
            result = position.x;
        }
        else if (dimension == DIM_Y)
        {
            result = position.y;
        }
        return result;
    } 
    inline float getMax(int dimension) const
    {
        float result = 0;
        if (dimension == DIM_X)
        {
            result = position.x + size.width;
        }
        else if (dimension == DIM_Y)
        {
            result = position.y + size.height;
        }
        return result;
    }

    bool collides(const Rect &rect);
    glm::vec2 getCentre() const;
};

#endif // RECT

