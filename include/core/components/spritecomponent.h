#ifndef SPRITECOMPONENT_H
#define SPRITECOMPONENT_H

#include "core/boiler.h"
#include "core/component.h"
#include "video/renderer.h"
#include "video/vertexdata.h"
#include "video/model.h"

class SpriteSheetFrame;

struct SpriteComponent : public Component
{
    const SpriteSheetFrame *spriteFrame;
    glm::vec4 color;
    std::shared_ptr<const Model> model;

	SpriteComponent(const Rect &frame)
	{
		// 2D vertex and texture coords
		const float sizeW = frame.size.getWidth();
		const float sizeH = frame.size.getHeight();

		VertexData vertData(
		{
			{ 0.0f, sizeH, 0.0f },
			{ sizeW, 0.0f, 0.0f },
			{ 0.0f, 0.0f, 0.0f },

			{ 0.0f, sizeH, 0.0f },
			{ sizeW, sizeH, 0.0f },
			{ sizeW, 0.0f, 0.0f }

		});
		model = Boiler::getInstance().getRenderer().loadModel(vertData);
		spriteFrame = nullptr;
	}
};

#endif /* SPRITECOMPONENT_H */
