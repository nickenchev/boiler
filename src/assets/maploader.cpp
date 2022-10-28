#include <iostream>
#include <string>
#include <vector>
#include <rapidjson/document.h>

#include "assets/gltfimporter.h"
#include "json/jsonloader.h"
#include "assets/maps/maploader.h"
#include "core/components/transformcomponent.h"
#include "core/components/rendercomponent.h"
#include "camera/cameracomponent.h"
#include "physics/movementcomponent.h"
#include "physics/collisioncomponent.h"
#include "physics/physicscomponent.h"
#include "input/inputcomponent.h"
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
				assetsLoaded.push_back(GLTFImporter(assetSet, engine, asset["file"].GetString()));
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
					const auto renderComponent = ecs.createComponent<RenderComponent>(entity);
					const int assetIndex = comp["assetIndex"].GetInt();
					const GLTFImporter &asset = assetsLoaded[assetIndex];

					childEntities = asset.createInstance(entity);

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
					const auto transformComponent = ecs.getComponentStore().hasComponent<TransformComponent>(entity)
						? &ecs.getComponentStore().retrieve<TransformComponent>(entity)
						: ecs.createComponent<TransformComponent>(entity).get();

					if (comp.HasMember("translation"))
					{
						transformComponent->setPosition(getVector(comp, "translation"));
					}

					if (comp.HasMember("orientation"))
					{
						const auto &orientation = comp["orientation"].GetObject();
						quat orientationQuat(orientation["w"].GetFloat(), orientation["x"].GetFloat(),
											 orientation["y"].GetFloat(), orientation["z"].GetFloat());
						transformComponent->setOrientation(orientationQuat);
					}
					if (comp.HasMember("scale"))
					{
						transformComponent->setScale(getVector(comp, "scale"));
					}
				}
				if (components.HasMember("physics"))
				{
					const auto &comp = components["physics"];
					auto physics = ecs.createComponent<PhysicsComponent>(entity);

					if (comp.HasMember("speed"))
					{
						physics->speed = comp["speed"].GetFloat();
					}
					if (comp.HasMember("acceleration"))
					{
						physics->acceleration = comp["acceleration"].GetFloat();
					}
				}
				if (components.HasMember("collision"))
				{
					const auto &comp = components["collision"];
					auto collisionComponent = ecs.createComponent<CollisionComponent>(entity);

					if (comp.HasMember("dynamic"))
					{
						collisionComponent->isDynamic = comp["dynamic"].GetBool();
					}

					if (comp.HasMember("aabb"))
					{
						collisionComponent->colliderType = ColliderType::AABB;
						const auto &volume = comp["aabb"];
						collisionComponent->min = getVector(volume, "min");
						collisionComponent->max = getVector(volume, "max");
					}
					else if (comp.HasMember("sphere"))
					{
						collisionComponent->colliderType = ColliderType::Sphere;
						const auto &volume = comp["sphere"];
						collisionComponent->min = getVector(volume, "min");
						collisionComponent->max = getVector(volume, "max");
					}
					else if (comp.HasMember("mesh"))
					{
						collisionComponent->colliderType = ColliderType::Mesh;
						assert(ecs.getComponentStore().hasComponent<RenderComponent>(entity));
						const auto &volume = comp["mesh"];
						const auto &renderComponent = ecs.getComponentStore().retrieve<RenderComponent>(entity);
						collisionComponent->mesh = renderComponent.mesh;

						for (Entity childEntity : childEntities)
						{
							if (ecs.getComponentStore().hasComponent<RenderComponent>(childEntity))
							{
								const RenderComponent &childRender = ecs.getComponentStore().retrieve<RenderComponent>(childEntity);
								auto childCollision = ecs.createComponent<CollisionComponent>(childEntity);
								childCollision->colliderType = ColliderType::Mesh;
								childCollision->mesh = childRender.mesh;
							}
						}
					}
				}
				if (components.HasMember("camera"))
				{
					const auto &comp = components["camera"];
					const auto cameraComponent = ecs.createComponent<CameraComponent>(entity);
					cameraComponent->direction = getVector(comp, "direction");
					cameraComponent->up = getVector(comp, "up");
				}
				if (components.HasMember("movement"))
				{
					const auto &comp = components["movement"];
					const auto moveComponent = ecs.createComponent<MovementComponent>(entity);
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
