#include "display/renderer.h"
#include "core/components/transformcomponent.h"
#include "core/components/rendercomponent.h"
#include "core/matrixcache.h"
#include "core/components/materialcomponent.h"
#include "core/components/parentcomponent.h"
#include "core/entitycomponentsystem.h"
#include "display/systems/rendersystem.h"
#include "display/materialgroup.h"

#include <thread>

using namespace Boiler;

RenderSystem::RenderSystem(MatrixCache &matrixCache) : System("Render System"), matrixCache(matrixCache)
{
	expects<TransformComponent>();
	expects<RenderComponent>();
}

void RenderSystem::update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, EntityComponentSystem &ecs)
{
	// TODO: none of these should be fixed sizes
	std::vector<MaterialGroup> materialGroups, alphaGroups, postDepthGroups;
	materialGroups.resize(512);
	alphaGroups.resize(512);
	postDepthGroups.resize(512);

	// calculate matrices and setup material groups
	for (unsigned int i = 0; i < static_cast<unsigned int>(getEntities().size()); ++i)
	{
		const Entity &entity = getEntities()[i];
        TransformComponent &transform = ecs.getComponentStore().retrieve<TransformComponent>(entity);
        RenderComponent &render = ecs.getComponentStore().retrieve<RenderComponent>(entity);

		if (!render.hidden)
		{
			// calculate model matrix
			glm::mat4 modelMatrix = matrixCache.getMatrix(entity, ecs.getComponentStore());
			Entity currentEntity = entity;

			const static Material defaultMaterial;
			const Mesh &mesh = assetSet.meshes.get(render.meshId);
			for (const auto &primitiveId : mesh.primitives)
			{
				const Primitive &primitive = assetSet.primitives(primitiveId);

				assert(primitive.materialId != -1);
				const Material &material = assetSet.materials(primitive.materialId);

				MaterialGroup *matGroup = &materialGroups[primitive.materialId];
				if (!material.depth)
				{
					matGroup = &postDepthGroups[primitive.materialId];
				}
				else if (material.alphaMode == AlphaMode::blend || material.alphaMode == AlphaMode::mask) // TODO: Mask should be handled with discard in GLSL
				{
					matGroup = &alphaGroups[primitive.materialId];
				}

				matGroup->materialId = primitive.materialId;

				if (primitive.materialId != Asset::noAsset())
				{
					AssetId matrixId = renderer.addMatrix(modelMatrix);
					matGroup->primitives.push_back(MaterialGroup::PrimitiveInstance(primitiveId, matrixId));
				}
			}
		}
	}

	renderer.render(assetSet, frameInfo, materialGroups, RenderStage::PRE_DEFERRED_LIGHTING);
	renderer.render(assetSet, frameInfo, alphaGroups, RenderStage::ALPHA_BLENDING);
	renderer.render(assetSet, frameInfo, postDepthGroups, RenderStage::POST_DEPTH_WRITE);
}
