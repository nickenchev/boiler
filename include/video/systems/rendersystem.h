#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "core/system.h"
#include "core/materialid.h"
#include "video/material.h"

namespace Boiler
{

class Renderer;

class RenderSystem : public System
{
	MaterialId materialId;
	Renderer &renderer;
	std::vector<Boiler::Material> materials;

public:
	RenderSystem(Renderer &renderer) : System("Render System"), renderer(renderer)
	{
		materialId = 1;
	}
	void update(ComponentStore &store, const double delta) override;

	MaterialId addMaterial(const Material &material);
	Material &getMaterial(MaterialId materialId);
};

}

#endif /* RENDERSYSTEM_H */
