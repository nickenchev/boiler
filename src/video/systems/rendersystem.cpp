#include "video/renderer.h"
#include "core/components/transformcomponent.h"
#include "core/components/rendercomponent.h"
#include "core/components/materialcomponent.h"
#include "core/components/parentcomponent.h"
#include "core/componentstore.h"
#include "video/systems/rendersystem.h"

#include <thread>

using namespace Boiler;


void RenderSystem::update(ComponentStore &store, const double)
{
	for (const Entity &entity : getEntities())
	{
		TransformComponent &transform = store.retrieve<TransformComponent>(entity);
		RenderComponent &render = store.retrieve<RenderComponent>(entity);

		// calculate model matrix
		glm::mat4 modelMatrix = transform.getMatrix();
		Entity currentEntity = entity;
		while (store.hasComponent<ParentComponent>(currentEntity))
		{
			ParentComponent &parentComp = store.retrieve<ParentComponent>(currentEntity);
			if (store.hasComponent<TransformComponent>(parentComp.entity))
			{
				auto &parentTransform = store.retrieve<TransformComponent>(parentComp.entity);
				modelMatrix = parentTransform.getMatrix() * modelMatrix;
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
