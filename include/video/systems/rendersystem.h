#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "core/system.h"
#include "core/materialid.h"
#include "core/lightsourceid.h"
#include "video/material.h"
#include "video/lightsource.h"

namespace Boiler
{

class Renderer;

class RenderSystem : public System
{
	MaterialId materialId;
	Renderer &renderer;
	std::vector<Material> materials;

public:
	RenderSystem(Renderer &renderer) : System("Render System"), renderer(renderer)
	{
		materialId = 1;
	}
	void update(ComponentStore &store, const double delta) override;

	// materials
	MaterialId addMaterial(const Material &material);
	std::vector<Boiler::Material> &getMaterials() { return materials; }
	Material &getMaterial(MaterialId materialId);
};

}

#endif /* RENDERSYSTEM_H */
