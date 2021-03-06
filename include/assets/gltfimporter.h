#ifndef GLTFIMPORTER_H
#define GLTFIMPORTER_H

#include <vector>
#include <string>

#include "core/asset.h"
#include "core/logger.h"
#include "assets/importresult.h"

#include "gltf.h"
#include "modelaccessors.h"

namespace Boiler
{
	class Engine;
	struct Entity;

	class GLTFImporter
	{
		Engine &engine;
		Logger logger;
		std::vector<std::vector<std::byte>> buffers;
		gltf::Model model;
		ImportResult result;

		Entity loadNode(std::vector<Entity> &nodeEntities, const Entity nodeEntity, int nodeIndex, const Entity parentEntity) const;
		Primitive loadPrimitive(Engine &engine, const gltf::ModelAccessors &modelAccess, const gltf::Primitive &primitive);

	public:
		GLTFImporter(Boiler::Engine &engine, const std::string &gltfPath);

        ImportResult import(const std::string &gltfPath);

		void createInstance(const Entity &rootEntity) const;

		const ImportResult &getImportResult() const { return result; }
	};
}
#endif /* GLTFIMPORTER_H */
