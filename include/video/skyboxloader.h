#ifndef SKYBOXLOADER_H
#define SKYBOXLOADER_H

#include <string>

namespace Boiler {

class Renderer;
class EntityComponentSystem;

class SkyBoxLoader
{
	Renderer &renderer;
	EntityComponentSystem &ecs;

public:
	SkyBoxLoader(Renderer &renderer, EntityComponentSystem &ecs) : renderer(renderer), ecs(ecs) {}

	void load(const std::string &top, const std::string &bottom,
			  const std::string &left, const std::string &right,
			  const std::string &front, const std::string &back);
};

}

#endif /* SKYBOXLOADER_H */
