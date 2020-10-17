#include "video/renderer.h"
#include "core/components/positioncomponent.h"
#include "core/components/rendercomponent.h"
#include "core/components/materialcomponent.h"
#include "core/components/parentcomponent.h"
#include "core/componentstore.h"
#include "video/systems/rendersystem.h"

using namespace Boiler;

void RenderSystem::update(ComponentStore &store, const double)
{
	// draw objects
	for (auto &entity : getEntities())
	{
		PositionComponent &pos = store.retrieve<PositionComponent>(entity);
		RenderComponent &render = store.retrieve<RenderComponent>(entity);

		// calculate model matrix
		glm::mat4 modelMatrix = pos.getMatrix();
		Entity currentEntity = entity;
		while (store.hasComponent<ParentComponent>(currentEntity))
		{
			ParentComponent &parentComp = store.retrieve<ParentComponent>(currentEntity);
			if (store.hasComponent<PositionComponent>(parentComp.entity))
			{
				PositionComponent &parentPos = store.retrieve<PositionComponent>(parentComp.entity);
				modelMatrix = parentPos.getMatrix() * modelMatrix;
			}
			currentEntity = parentComp.entity;
		}

		for (const auto &primitive : render.mesh.primitives)
		{
			unsigned int matIndex = primitive.materialId - 1;
			assert(matIndex >= 0 && matIndex < materials.size());

			const Material &material = materials[matIndex];
			renderer.render(modelMatrix, primitive, material);
		}
	}
}

MaterialId RenderSystem::addMaterial(const Material &material)
{
	materials.push_back(material);
	MaterialId newId = materialId++;

	logger.log("Added material with ID: {}", newId);
	return newId;
}
Material &RenderSystem::getMaterial(MaterialId materialId)
{
	return materials[materialId - 1];
}
