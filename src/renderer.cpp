#include "renderer.h"

Renderer::Renderer(std::string name, Engine &engine) : Component(name, engine), clearColor(0, 0, 0), globalScale(1.0f, 1.0f)
{
    camera = nullptr;
}
