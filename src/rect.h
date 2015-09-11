#ifndef RECT
#define RECT

#define GLM_COMPILER 0
#include <glm/glm.hpp>

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

    float getMinX() const { return position.x; }
    float getMaxX() const { return position.x + size.width; }
    float getMinY() const { return position.y; }
    float getMaxY() const { return position.y + size.height; }

    bool collides(const Rect &rect);
    glm::vec2 getCentre() const;
};

#endif // RECT

