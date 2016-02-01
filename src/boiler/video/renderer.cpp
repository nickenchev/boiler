#include "renderer.h"

Renderer::Renderer(std::string name) : Component(name), clearColor(0, 0, 0), globalScale(1.0f, 1.0f)
{
    camera = nullptr;
}
