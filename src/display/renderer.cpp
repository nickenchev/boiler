#include "display/renderer.h"

using namespace Boiler;

Renderer::Renderer(std::string name) : clearColor(0, 0, 0), globalScale(1.0f, 1.0f), logger(name)
{
	assetId = 0;
	frameLightIdx = 0;
}

void Renderer::initialize(const Size &size)
{
	setScreenSize(size);
}

void Renderer::resize(const Size &size)
{
	setScreenSize(size);
}

bool Renderer::prepareFrame(const FrameInfo &frameInfo)
{
	frameLightIdx = 0;
	return true;
}

