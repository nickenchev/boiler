#include "display/renderer.h"
#include "core/components/transformcomponent.h"
#include "core/components/rendercomponent.h"
#include "core/components/materialcomponent.h"
#include "core/components/parentcomponent.h"
#include "core/componentstore.h"
#include "display/systems/rendersystem.h"
#include "display/materialgroup.h"

#include <thread>

using namespace Boiler;

RenderSystem::RenderSystem(Renderer &renderer) : System("Render System"), renderer(renderer)
{
}

void RenderSystem::update(FrameInfo frameInfo, ComponentStore &store)
{
	std::vector<MaterialGroup> materialGroups, postLightGroups;
	materialGroups.resize(256);
	postLightGroups.resize(256);
	std::vector<mat4> matrices(getEntities().size());

	// calculate matrices and setup material groups
	for (unsigned int i = 0; i < static_cast<unsigned int>(getEntities().size()); ++i)
	{
		const Entity &entity = getEntities()[i];
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
		matrices[i] = modelMatrix;

		const static Material defaultMaterial(0);

		for (const auto &primitive : render.mesh.primitives)
		{
			const Material &material = renderer.getMaterial(primitive.materialId);
			auto &matGroup = material.depth ? materialGroups[primitive.materialId] : postLightGroups[primitive.materialId];
			matGroup.materialId = primitive.materialId;

			if (primitive.materialId != Asset::NO_ASSET)
			{
				matGroup.primitives.push_back(MaterialGroup::PrimitiveInstance(primitive, i));
			}
		}
	}

	renderer.render(frameInfo, matrices, materialGroups, postLightGroups);
}
