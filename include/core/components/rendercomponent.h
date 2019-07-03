#ifndef RENDERCOMPONENT_H
#define RENDERCOMPONENT_H

#include <glm/glm.hpp>
#include <memory>
#include "core/componenttype.h"
#include "video/model.h"

struct RenderComponent : public Boiler::ComponentType<RenderComponent>
{
	std::shared_ptr<const Boiler::Model> model;
    glm::vec4 colour;

	RenderComponent(const std::shared_ptr<const Boiler::Model> model)
	{
		this->model = model;
		this->colour = glm::vec4(1, 1, 1, 1);
	}
};

#endif /* RENDERCOMPONENT_H */
