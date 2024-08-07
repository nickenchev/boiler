#pragma once

#include <vector>
#include <gltf.h>
#include "importresult.h"
#include "core/logger.h"
#include "assets/assetset.h"

namespace Boiler
{

class EntityComponentSystem;

class GLTFModel
{
    static Logger logger;
    const std::string filePath;
    const std::vector<std::vector<std::byte>> buffers;
    const gltf::Model model;
    const ImportResult result;

public:
    GLTFModel(const std::string &filePath, const std::vector<std::vector<std::byte>> &&buffers, gltf::Model &&model, ImportResult &&result);

    const gltf::Model &getModel() const { return model; }
    const std::string &getFilePath() const { return filePath; }
	const ImportResult &getImportResult() const { return result; }
    std::vector<Entity> createInstance(const AssetSet &assetSet, EntityComponentSystem &ecs, const Entity &rootEntity) const;
    Entity loadNode(const AssetSet &assetSet, EntityComponentSystem &ecs, std::vector<Entity> &nodeEntities, const Entity nodeEntity, int nodeIndex, const Entity parentEntity) const;
};

}

