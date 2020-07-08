#include "video/renderer.h"
#include "core/components/positioncomponent.h"
#include "core/components/rendercomponent.h"
#include "core/components/parentcomponent.h"
#include "core/componentstore.h"
#include "video/systems/rendersystem.h"

using namespace Boiler;

struct RenderObject
{
	Entity entity;
	PositionComponent positionComponent;
	RenderComponent renderComponent;
};

void RenderSystem::update(ComponentStore &store, const double delta)
{
	std::vector<RenderObject> renderList;
	for (auto &entity : getEntities())
	{
		PositionComponent &pos = store.retrieve<PositionComponent>(entity);
		RenderComponent &render = store.retrieve<RenderComponent>(entity);

		renderList.push_back(RenderObject({
			entity,
			pos,
			render
		}));

		glm::mat4 modelMatrix = pos.getMatrix();

		// iterate over the parent chain and setup the model matrix
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

		for (const auto &mesh : render.meshes)
		{
			renderer.render(modelMatrix, mesh.model,
							mesh.spriteSheetFrame.getSourceTexture(),
							nullptr, mesh.colour);
		}
	}
}
