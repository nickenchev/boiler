#include "display/renderer.h"

using namespace Boiler;

Renderer::Renderer(std::string name, unsigned short maxFramesInFlight) : clearColor(0, 0, 0), globalScale(1.0f, 1.0f), logger(name)
{
	frameLightIdx = 0;
	this->maxFramesInFlight = maxFramesInFlight;
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
	matrices.reset();
	return true;
}

AssetId Renderer::addMatrix(mat4 matrix)
{
	return matrices.add(std::move(matrix));
}
