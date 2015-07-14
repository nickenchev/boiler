#ifndef RECT
#define RECT

struct Vector2
{
    int x, y;
    Vector2() { x = 0; y = 0; }
    Vector2(int x, int y) : x(x), y(y) { }
};

struct Size
{
    int width, height;
    Size() { width = 0; height = 0; }
    Size(int width, int height) : width(width), height(height) { }
};

struct Rect
{
    Vector2 position;
    Size size;
};

#endif // RECT

