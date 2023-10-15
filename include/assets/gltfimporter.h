#pragma once

#include <vector>
#include <string>
#include <memory>
#include <gltf.h>

#include "core/asset.h"
#include "core/logger.h"
#include "display/vertexdata.h"
#include "assets/importresult.h"

#include "modelaccessors.h"
#include "assets/gltfmodel.h"

namespace Boiler
{

class Engine;
class Entity;
struct AssetSet;

class GLTFImporter
{
	Engine &engine;
	Logger logger;

    Entity loadNode(const gltf::Model &model, std::vector<Entity> &nodeEntities, const Entity nodeEntity, int nodeIndex, const Entity parentEntity) const;
	VertexData getPrimitiveData(Engine &engine, const gltf::ModelAccessors &modelAccess, const gltf::Primitive &primitive);
	std::string fixSpaces(std::string input);
	AssetId loadImage(const gltf::Model &model, int imageIndex, std::vector<AssetId> textureIds);

public:
    GLTFImporter(Boiler::Engine &engine);

    std::shared_ptr<GLTFModel> import(AssetSet &assetSet, const std::string& gltfPath);
	std::vector<Entity> createInstance(const Entity &rootEntity) const;
};

}
