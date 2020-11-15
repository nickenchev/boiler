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

		const Material defaultMaterial(0);
		for (const auto &primitive : render.mesh.primitives)
		{
			const Material &material = primitive.materialId != 0
				? renderer.getMaterial(primitive.materialId) : defaultMaterial;
			renderer.render(modelMatrix, primitive, material);
		}
	}
}
