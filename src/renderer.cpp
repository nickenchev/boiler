#include "renderer.h"

Renderer::Renderer(std::string name, Engine &engine) : Component(name, engine)
{
    //setup global scaling
    globalScale = glm::vec2(1.0f, 1.0f);

    camera = nullptr;
}
