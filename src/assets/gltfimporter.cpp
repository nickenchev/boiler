#include <fstream>
#include <sstream>

#include "assets/gltfimporter.h"
#include "gltf.h"

using namespace Boiler;

GLTFImporter::GLTFImporter(EntityComponentSystem &ecs) : ecs(ecs)
{
}

void GLTFImporter::import(std::string path)
{
	std::ifstream infile(path);
	std::stringstream buffer;
	buffer << infile.rdbuf();
	const std::string jsonString = buffer.str();
	infile.close();

	auto model = gltf::load(jsonString);
}
