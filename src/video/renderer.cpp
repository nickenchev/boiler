#include "video/renderer.h"

using namespace Boiler;

Renderer::Renderer(std::string name) : logger(name), clearColor(0, 0, 0), globalScale(1.0f, 1.0f)
{
	assetId = 1;
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

AssetId Renderer::nextAssetId()
{
	return assetId++;
}

void Renderer::addLightSource(const LightSource &lightSource)
{
	lightSources[frameLightIdx++] = lightSource;
}

void Renderer::beginRender()
{
	frameLightIdx = 0;
}
