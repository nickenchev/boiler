#pragma once

#include <vector>
#include "importresult.h"
#include "gltf.h"
#include "core/logger.h"

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

    const std::string &getFilePath() const { return filePath; }
	const ImportResult &getImportResult() const { return result; }
    std::vector<Entity> createInstance(EntityComponentSystem &ecs, const Entity &rootEntity) const;
    Entity loadNode(EntityComponentSystem &ecs, std::vector<Entity> &nodeEntities, const Entity nodeEntity, int nodeIndex, const Entity parentEntity) const;
};

}

