#include "video/renderer.h"

using namespace Boiler;

Renderer::Renderer(std::string name) : logger(name), clearColor(0, 0, 0), globalScale(1.0f, 1.0f)
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

AssetId Renderer::nextAssetId()
{
	return assetId++;
}

void Renderer::beginRender()
{
	frameLightIdx = 0;
}

Material &Renderer::createMaterial()
{
	materials.push_back(Material(nextAssetId()));
	Material &material = materials[materials.size() - 1];

	logger.log("Added material with asset ID: {}", material.getAssetId());
	return material;
}
Material &Renderer::getMaterial(AssetId assetId)
{
	auto material = std::find(materials.begin(), materials.end(), assetId);
	assert(material != materials.end());
	return *material;
}
