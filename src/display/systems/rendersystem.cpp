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

RenderSystem::RenderSystem() : System("Render System")
{
	expects<TransformComponent>();
	expects<RenderComponent>();
}

void RenderSystem::update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, ComponentStore &store)
{
	std::vector<MaterialGroup> materialGroups, postLightGroups;
	materialGroups.resize(256);
	postLightGroups.resize(256);

	// calculate matrices and setup material groups
	for (unsigned int i = 0; i < static_cast<unsigned int>(getEntities().size()); ++i)
	{
		const Entity &entity = getEntities()[i];
		TransformComponent &transform = store.retrieve<TransformComponent>(entity);
		RenderComponent &render = store.retrieve<RenderComponent>(entity);

		// calculate model matrix
		glm::mat4 modelMatrix = transform.getMatrix();
		Entity currentEntity = entity;

		// calculate with parent transformations
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

		const static Material defaultMaterial;
		for (const auto &primitiveId : render.mesh.primitives)
		{
			const Primitive &primitive = assetSet.primitives.get(primitiveId);
			const Material &material = assetSet.materials.get(primitive.materialId);
			auto &matGroup = material.depth ? materialGroups[primitive.materialId] : postLightGroups[primitive.materialId];
			matGroup.materialId = primitive.materialId;

			if (primitive.materialId != Asset::NO_ASSET)
			{
				AssetId matrixId = renderer.addMatrix(modelMatrix);
				matGroup.primitives.push_back(MaterialGroup::PrimitiveInstance(primitiveId, matrixId));
			}
		}
	}

	renderer.render(assetSet, frameInfo, materialGroups, RenderStage::PRE_DEFERRED_LIGHTING);
	renderer.render(assetSet, frameInfo, postLightGroups, RenderStage::POST_DEPTH_WRITE);
}
