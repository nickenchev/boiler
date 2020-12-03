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
	bool cached[2000]{false};
	mat4 matrices[2000];

	auto getMatrix = [&store, &cached, &matrices](const Entity entity)
	{
		if (!cached[entity.getId() - 1])
		{
			PositionComponent &pos = store.retrieve<PositionComponent>(entity);
			matrices[entity.getId() - 1] = pos.getMatrix();
			cached[entity.getId() - 1] = true;
		}
		return matrices[entity.getId() - 1];
	};

	// draw objects
	for (auto &entity : getEntities())
	{
		RenderComponent &render = store.retrieve<RenderComponent>(entity);

		// calculate model matrix
		glm::mat4 modelMatrix = getMatrix(entity);
		Entity currentEntity = entity;
		while (store.hasComponent<ParentComponent>(currentEntity))
		{
			ParentComponent &parentComp = store.retrieve<ParentComponent>(currentEntity);
			if (store.hasComponent<PositionComponent>(parentComp.entity))
			{
				modelMatrix = getMatrix(parentComp.entity) * modelMatrix;
			}
			currentEntity = parentComp.entity;
		}

		const static Material defaultMaterial(0);
		for (const auto &primitive : render.mesh.primitives)
		{
			const Material &material = primitive.materialId != 0
				? renderer.getMaterial(primitive.materialId) : defaultMaterial;

			renderer.render(modelMatrix, primitive, material);
		}
	}
}
