#include <iostream>
#include <string>
#include <vector>
#include <rapidjson/document.h>
#include <rapidjson/encodings.h>
#include <rapidjson/rapidjson.h>

#include "assets/gltfimporter.h"
#include "json/jsonloader.h"
#include "assets/maps/maploader.h"
#include "core/components/transformcomponent.h"
#include "core/components/rendercomponent.h"
#include "camera/cameracomponent.h"
#include "physics/movementcomponent.h"
#include "physics/collidercomponent.h"
#include "physics/physicscomponent.h"
#include "input/inputcomponent.h"

using namespace Boiler;
using namespace rapidjson;

vec3 getVector(const GenericValue<UTF8<>> &object, const std::string &name);

void MapLoader::load(const std::string &filePath)
{
	const std::string jsonData = JsonLoader::loadJson(filePath);
	Document doc;
	doc.Parse(jsonData.c_str());

	// preload all assets
    GLTFImporter gltfImporter(engine);
    std::vector<std::shared_ptr<GLTFModel>> assetsLoaded;

	if (doc.HasMember("assets"))
	{
		const auto &assets = doc["assets"].GetArray();
		for (const auto &asset : assets)
		{
			if (strcmp(asset["type"].GetString(), "gltf") == 0)
			{
                assetsLoaded.push_back(gltfImporter.import(assetSet, asset["file"].GetString()));
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
			std::string name = ent.HasMember("name") ? ent["name"].GetString() : "Unnamed";
			Entity entity = ecs.newEntity(name);
			if (ent.HasMember("components"))
			{
				const auto &components = ent["components"].GetObject();
				std::vector<Entity> childEntities;
				if (components.HasMember("render"))
				{
					const auto &comp = components["render"];
					const auto &renderComponent = ecs.createComponent<RenderComponent>(entity);
					const int assetIndex = comp["assetIndex"].GetInt();
                    const GLTFModel &asset = *assetsLoaded[assetIndex];

                    childEntities = gltfImporter.createInstance(entity);

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
				if (components.HasMember("transform"))
				{
					const auto &comp = components["transform"];
					auto &transformComponent = ecs.getComponentStore().hasComponent<TransformComponent>(entity)
						? ecs.getComponentStore().retrieve<TransformComponent>(entity)
						: ecs.createComponent<TransformComponent>(entity);

					if (comp.HasMember("translation"))
					{
						transformComponent.translation = getVector(comp, "translation");
					}

					if (comp.HasMember("orientation"))
					{
						const auto &orientation = comp["orientation"].GetObject();
						quat orientationQuat(orientation["w"].GetFloat(), orientation["x"].GetFloat(),
											 orientation["y"].GetFloat(), orientation["z"].GetFloat());
						transformComponent.orientation = orientationQuat;
					}
					if (comp.HasMember("scale"))
					{
						transformComponent.scale = getVector(comp, "scale");
					}
				}
				if (components.HasMember("physics"))
				{
					const auto &comp = components["physics"];
					auto &physics = ecs.createComponent<PhysicsComponent>(entity);

					if (comp.HasMember("speed"))
					{
						physics.speed = comp["speed"].GetFloat();
					}
					if (comp.HasMember("acceleration"))
					{
						physics.acceleration = comp["acceleration"].GetFloat();
					}
				}
				if (components.HasMember("collision"))
				{
					const auto &comp = components["collision"];
					auto &colliderComponent = ecs.createComponent<ColliderComponent>(entity);

					if (comp.HasMember("dynamic"))
					{
						colliderComponent.isDynamic = comp["dynamic"].GetBool();
					}

					if (comp.HasMember("aabb"))
					{
						colliderComponent.colliderType = ColliderType::AABB;
						const auto &volume = comp["aabb"];
						colliderComponent.min = getVector(volume, "min");
						colliderComponent.max = getVector(volume, "max");
					}
					else if (comp.HasMember("sphere"))
					{
						colliderComponent.colliderType = ColliderType::Sphere;
						const auto &volume = comp["sphere"];
						colliderComponent.min = getVector(volume, "min");
						colliderComponent.max = getVector(volume, "max");
					}
					else if (comp.HasMember("mesh"))
					{
						colliderComponent.colliderType = ColliderType::Mesh;
						assert(ecs.getComponentStore().hasComponent<RenderComponent>(entity));
						const auto &volume = comp["mesh"];
						const auto &renderComponent = ecs.getComponentStore().retrieve<RenderComponent>(entity);
						colliderComponent.meshId = renderComponent.meshId;

						for (Entity childEntity : childEntities)
						{
							if (ecs.getComponentStore().hasComponent<RenderComponent>(childEntity))
							{
								const RenderComponent &childRender = ecs.getComponentStore().retrieve<RenderComponent>(childEntity);
								auto &childCollision = ecs.createComponent<ColliderComponent>(childEntity);
								childCollision.colliderType = ColliderType::Mesh;
								childCollision.meshId = childRender.meshId;
							}
						}
					}
				}
				if (components.HasMember("camera"))
				{
					const auto &comp = components["camera"];
					auto &cameraComponent = ecs.createComponent<CameraComponent>(entity);
					cameraComponent.up = getVector(comp, "up");
				}
				if (components.HasMember("movement"))
				{
					const auto &comp = components["movement"];
					auto &moveComponent = ecs.createComponent<MovementComponent>(entity);
				}
				if (components.HasMember("input"))
				{
					ecs.createComponent<InputComponent>(entity);
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
