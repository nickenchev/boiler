#include "display/renderer.h"
#include "core/components/transformcomponent.h"
#include "core/components/rendercomponent.h"
#include "core/components/materialcomponent.h"
#include "core/components/parentcomponent.h"
#include "core/entitycomponentsystem.h"
#include "display/systems/rendersystem.h"
#include "display/materialgroup.h"

#include <thread>

using namespace Boiler;

RenderSystem::RenderSystem() : System("Render System")
{
	expects<TransformComponent>();
	expects<RenderComponent>();
}

mat4 getMatrix(Entity entity, ComponentStore &store, std::array<std::optional<mat4>, 5000> &matrixCache)
{
	glm::mat4 modelMatrix;
	std::optional<mat4> &opt = matrixCache[entity.getId()];
	if (opt.has_value())
	{
		modelMatrix = opt.value();
	}
	else if (store.hasComponent<TransformComponent>(entity))
	{
		TransformComponent &transform = store.retrieve<TransformComponent>(entity);
		modelMatrix = transform.getMatrix();
		if (store.hasComponent<ParentComponent>(entity))
		{
			ParentComponent &parentComp = store.retrieve<ParentComponent>(entity);
			modelMatrix = getMatrix(parentComp.entity, store, matrixCache) * modelMatrix;
		}
		matrixCache[entity.getId()] = modelMatrix;
	}

	return modelMatrix;
}

void RenderSystem::update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, EntityComponentSystem &ecs)
{
	// TODO: none of these can be fixed sizes
	std::vector<MaterialGroup> materialGroups, alphaGroups, postDepthGroups;
	materialGroups.resize(512);
	alphaGroups.resize(512);
	postDepthGroups.resize(512);

	// TODO: Can't be fixed size
	std::array<std::optional<mat4>, 5000> matrixCache;

	// calculate matrices and setup material groups
	for (unsigned int i = 0; i < static_cast<unsigned int>(getEntities().size()); ++i)
	{
		const Entity &entity = getEntities()[i];
        TransformComponent &transform = ecs.getComponentStore().retrieve<TransformComponent>(entity);
        RenderComponent &render = ecs.getComponentStore().retrieve<RenderComponent>(entity);

		// calculate model matrix
        glm::mat4 modelMatrix = getMatrix(entity, ecs.getComponentStore(), matrixCache);
		Entity currentEntity = entity;

		const static Material defaultMaterial;
		for (const auto &primitiveId : render.mesh.primitives)
		{
			const Primitive &primitive = assetSet.primitives.get(primitiveId);
			const Material &material = assetSet.materials.get(primitive.materialId);

			MaterialGroup *matGroup = &materialGroups[primitive.materialId];
			if (!material.depth)
			{
				matGroup = &postDepthGroups[primitive.materialId];
			}
			else if (material.alphaMode == AlphaMode::BLEND || material.alphaMode == AlphaMode::MASK) // TODO: Mask should be handled with discard
			{
				matGroup = &alphaGroups[primitive.materialId];
			}

			matGroup->materialId = primitive.materialId;

			if (primitive.materialId != Asset::NO_ASSET)
			{
				AssetId matrixId = renderer.addMatrix(modelMatrix);
				matGroup->primitives.push_back(MaterialGroup::PrimitiveInstance(primitiveId, matrixId));
			}
		}
	}

	renderer.render(assetSet, frameInfo, materialGroups, RenderStage::PRE_DEFERRED_LIGHTING);
	renderer.render(assetSet, frameInfo, alphaGroups, RenderStage::ALPHA_BLENDING);
	renderer.render(assetSet, frameInfo, postDepthGroups, RenderStage::POST_DEPTH_WRITE);
}
