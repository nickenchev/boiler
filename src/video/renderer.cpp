#include "video/renderer.h"

using namespace Boiler;

Renderer::Renderer(std::string name) : logger(name), clearColor(0, 0, 0), globalScale(1.0f, 1.0f)
{
}

void Renderer::initialize(const Size &size)
{
	setScreenSize(size);
}
