#include "core/components/positioncomponent.h"
#include "core/components/spritecomponent.h"
#include "core/components/parentcomponent.h"
#include "core/componentstore.h"
#include "video/systems/rendersystem.h"

using namespace Boiler;

void RenderSystem::update(ComponentStore &store, const double delta)
{
	for (auto &entity : getEntities())
	{
		PositionComponent &pos = store.retrieve<PositionComponent>(entity);
		SpriteComponent &sprite = store.retrieve<SpriteComponent>(entity);

		glm::mat4 modelMatrix = pos.getMatrix();

		// iterate over the parent chain and setup the model matrix
		Entity currentEntity = entity;
		while (store.hasComponent<ParentComponent>(currentEntity))
		{
			ParentComponent &parentComp = store.retrieve<ParentComponent>(currentEntity);
			PositionComponent &parentPos = store.retrieve<PositionComponent>(parentComp.entity);
			modelMatrix = parentPos.getMatrix() * modelMatrix;
			currentEntity = parentComp.entity;
		}

		renderer.render(modelMatrix, sprite.model, sprite.spriteFrame->getSourceTexture(),
						sprite.spriteFrame->getTextureInfo().get(), sprite.colour);
	}
}
