#include <iostream>
#include <string>
#include <vector>
#include <rapidjson/document.h>

#include "assets/gltfimporter.h"
#include "json/jsonloader.h"
#include "assets/maps/maploader.h"
#include "core/components/transformcomponent.h"
#include "core/components/rendercomponent.h"
#include "physics/collisioncomponent.h"
#include "rapidjson/encodings.h"
#include "rapidjson/rapidjson.h"

using namespace Boiler;

vec3 getVector(const GenericValue<UTF8<>> &object, const std::string &name);

void MapLoader::load(const std::string &filePath)
{
	using namespace rapidjson;

	const std::string jsonData = JsonLoader::loadJson(filePath);
	Document doc;
	doc.Parse(jsonData.c_str());

	// preload all assets
	std::vector<GLTFImporter> assetsLoaded;
	if (doc.HasMember("assets"))
	{
		const auto &assets = doc["assets"].GetArray();
		for (const auto &asset : assets)
		{
			if (strcmp(asset["type"].GetString(), "gltf") == 0)
			{
				assetsLoaded.push_back(GLTFImporter(engine, asset["file"].GetString()));
			}
		}
	}

	// create required entities
	EntityComponentSystem &ecs = engine.getEcs();
	if (doc.HasMember("entities"))
	{
		const auto &entities = doc["entities"].GetArray();
		for (const auto &ent : entities)
		{
			Entity entity = ecs.newEntity();
			if (ent.HasMember("components"))
			{
				const auto &components = ent["components"].GetArray();
				for (const auto &comp : components)
				{
					if (strcmp(comp["type"].GetString(), "render") == 0)
					{
						const auto renderComponent = ecs.createComponent<RenderComponent>(entity);
						const int assetIndex = comp["assetIndex"].GetInt();
						const GLTFImporter &asset = assetsLoaded[assetIndex];

						asset.createInstance(entity);

						if (asset.getImportResult().animations.size())
						{
							// load animations
							auto &animComp = ecs.getComponentStore().retrieve<AnimationComponent>(entity);
							for (AnimationId animId : asset.getImportResult().animations)
							{
								animComp.addClip(Clip(0, animId, true));
							}
						}
					}
					else if (strcmp(comp["type"].GetString(), "transform") == 0)
					{
						const auto transformComponent = ecs.createComponent<TransformComponent>(entity);

						transformComponent->setPosition(getVector(comp, "translation"));

						const auto &orientation = comp["orientation"].GetObject();
						quat orientationQuat(
							orientation["w"].GetFloat(),
							orientation["x"].GetFloat(),
							orientation["y"].GetFloat(),
							orientation["z"].GetFloat()
						);
						transformComponent->setOrientation(orientationQuat);

						transformComponent->setScale(getVector(comp, "scale"));
					}
					else if (strcmp(comp["type"].GetString(), "collision") == 0)
					{
						const auto collisionComponent = ecs.createComponent<CollisionComponent>(entity);

						// AABB
						if (comp.HasMember("aabb"))
						{
							const auto &volume = comp["aabb"];
							const auto collisionComponent = ecs.createComponent<CollisionComponent>(entity);
							collisionComponent->min = getVector(volume, "min");
							collisionComponent->max = getVector(volume, "max");
						}
					}
				}
			}
		}
	}
}

vec3 getVector(const GenericValue<UTF8<>> &object, const std::string &name)
{
	const auto &vecObj = object[name.c_str()].GetObject();
	return vec3(vecObj["x"].GetFloat(), vecObj["y"].GetFloat(), vecObj["z"].GetFloat());
}
