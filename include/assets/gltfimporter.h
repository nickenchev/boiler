#ifndef GLTFIMPORTER_H
#define GLTFIMPORTER_H

#include <string>

namespace Boiler
{

class EntityComponentSystem;

class GLTFImporter
{
	EntityComponentSystem &ecs;

public:
    GLTFImporter(EntityComponentSystem &ecs);

	void import(std::string path);
};

}
#endif /* GLTFIMPORTER_H */
