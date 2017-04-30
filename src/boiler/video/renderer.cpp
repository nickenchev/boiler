#include "video/renderer.h"

Renderer::Renderer(std::string name) : logger(name), clearColor(0, 0, 0), globalScale(1.0f, 1.0f)
{
    camera = nullptr;
}
