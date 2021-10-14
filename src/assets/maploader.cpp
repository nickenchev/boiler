#include <iostream>
#include <rapidjson/document.h>
#include <vector>

#include "assets/gltfimporter.h"
#include "json/jsonloader.h"
#include "assets/maps/maploader.h"
#include "core/components/transformcomponent.h"
#include "core/components/rendercomponent.h"

using namespace Boiler;

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

						assetsLoaded[assetIndex].createInstance(entity);
					}
					else if (strcmp(comp["type"].GetString(), "transform") == 0)
					{
						const auto transformComponent = ecs.createComponent<TransformComponent>(entity);

						const auto &translation = comp["translation"].GetObject();
						transformComponent->setPosition(
							translation["x"].GetFloat(),
							translation["y"].GetFloat(),
							translation["z"].GetFloat()
						);

						const auto &orientation = comp["orientation"].GetObject();
						quat orientationQuat(
							orientation["w"].GetFloat(),
							orientation["x"].GetFloat(),
							orientation["y"].GetFloat(),
							orientation["z"].GetFloat()
						);
						transformComponent->setOrientation(orientationQuat);

						const auto &scale = comp["scale"].GetObject();
						transformComponent->setScale(
							scale["x"].GetFloat(),
							scale["y"].GetFloat(),
							scale["z"].GetFloat()
						);
					}
				}
			}
		}
	}
}
