#ifndef MATRIXCACHE_H
#define MATRIXCACHE_H

#include <array>
#include <optional>
#include "core/math.h"
#include "core/componentstore.h"
#include "core/entity.h"
#include "core/components/transformcomponent.h"
#include "core/components/parentcomponent.h"

namespace Boiler
{

class MatrixCache
{
	// TODO: This size should be dynamic
	std::array<std::optional<mat4>, 5000> matrixCache;

public:
	void reset()
	{
		for (auto &opt : matrixCache)
		{
			opt.reset();
		}
	}

	mat4 getMatrix(Entity entity, ComponentStore &store)
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
				modelMatrix = getMatrix(parentComp.entity, store) * modelMatrix;
			}
			//matrixCache[entity.getId()] = modelMatrix;
		}

		return modelMatrix;
	}
};

};

#endif /* MATRIXCACHE_H */
