#ifndef SKYBOXLOADER_H
#define SKYBOXLOADER_H

#include <string>
#include "core/engine.h"
#include "assets/assetset.h"

namespace Boiler {

class Renderer;
class EntityComponentSystem;

class SkyBoxLoader
{
	AssetSet &assetSet;
	Renderer &renderer;
	EntityComponentSystem &ecs;

public:
	SkyBoxLoader(AssetSet &assetSet, Renderer &renderer, EntityComponentSystem &ecs) : assetSet(assetSet), renderer(renderer), ecs(ecs) {}

	Entity load(const std::string &top, const std::string &bottom,
				const std::string &left, const std::string &right,
				const std::string &front, const std::string &back);
};

}

#endif /* SKYBOXLOADER_H */
