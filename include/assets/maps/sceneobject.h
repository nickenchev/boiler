#pragma once

#include <string>

namespace Boiler::Editor {

	enum class ObjectType
	{
		Mesh,
		Camera,
		Light
	};

	class SceneObject
	{
		std::string name;
		const Entity entity;

	public:
		SceneObject(const std::string &name, Entity entity) : entity(entity)
		{
			this->name = name;
		}

		std::string getName() const { return name; }
		void setName(std::string &name) { this->name = name; }

		Entity getEntity() const { return entity; }

		virtual ObjectType getType() const = 0;
	};

	class SceneMesh : public SceneObject
	{
	public:
		SceneMesh(const std::string &name, Entity entity) : SceneObject(name, entity) { }
		ObjectType getType() const override { return ObjectType::Mesh; }
	};

	class SceneLight : public SceneObject
	{
	public:
		SceneLight(const std::string &name, Entity entity) : SceneObject(name, entity) { }
		ObjectType getType() const override { return ObjectType::Light; }
	};

	class SceneCamera : public SceneObject
	{
	public:
		SceneCamera(const std::string &name, Entity entity) : SceneObject(name, entity) { }
		ObjectType getType() const override { return ObjectType::Camera; }
	};
};
