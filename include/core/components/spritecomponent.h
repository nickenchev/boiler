#ifndef SPRITECOMPONENT_H
#define SPRITECOMPONENT_H

#include "core/engine.h"
#include "core/componenttype.h"
#include "video/renderer.h"
#include "video/vertexdata.h"
#include "video/model.h"

namespace Boiler
{

class SpriteSheetFrame;

struct SpriteComponent : public ComponentType<SpriteComponent>
{
    const SpriteSheetFrame *spriteFrame;
    glm::vec4 colour;
    std::shared_ptr<const Model> model;

	SpriteComponent(const Rect &frame) : colour(1.0f, 1.0f, 1.0f, 1.0f)
	{
		// 2D vertex and texture coords
		const float sizeW = frame.size.width;
		const float sizeH = frame.size.height;

		VertexData vertData(
		{
			{ 0.0f, sizeH, 0.0f },
			{ sizeW, 0.0f, 0.0f },
			{ 0.0f, 0.0f, 0.0f },

			{ 0.0f, sizeH, 0.0f },
			{ sizeW, sizeH, 0.0f },
			{ sizeW, 0.0f, 0.0f }

		});
		model = Engine::getInstance().getRenderer().loadModel(vertData);
		spriteFrame = nullptr;
	}
};

}

#endif /* SPRITECOMPONENT_H */
