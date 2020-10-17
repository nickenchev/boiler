#ifndef GLTFIMPORTER_H
#define GLTFIMPORTER_H

#include <string>
#include <unordered_map>

#include "core/asset.h"
#include "core/logger.h"

namespace Boiler
{
	class Engine;
	struct Entity;

	namespace gltf
	{
		struct Model;
		struct Primitive;
		class ModelAccessors;
	}

	class GLTFImporter
	{
		Logger logger;
		std::vector<Boiler::AssetId> assetIds;

	public:
		GLTFImporter() : logger("GLTF Importer") { }

		void import(Boiler::Engine &engine, std::string gltfPath);

		Entity loadNode(Engine &engine, const gltf::Model &model, const gltf::ModelAccessors &modelAccess,
						std::unordered_map<int, Entity> &nodeEntities, int nodeIndex);
		auto loadPrimitive(Engine &engine, const gltf::ModelAccessors &modelAccess, const gltf::Primitive &primitive);
	};
}
#endif /* GLTFIMPORTER_H */
