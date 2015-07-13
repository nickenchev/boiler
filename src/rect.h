#ifndef RECT
#define RECT

struct Vector2
{
    int x, y;
    Vector2() { x = 0; y = 0; }
};

struct Size
{
    int width, height;
    Size() { width = 0; height = 0; }
};

struct Rect
{
    Vector2 position;
    Size size;
};

#endif // RECT

