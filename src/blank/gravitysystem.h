#ifndef GRAVITYSYSTEM_H
#define GRAVITYSYSTEM_H

#include "core/system.h"
#include "core/components/positioncomponent.h"

class GravitySystem : public System
{
public:
	GravitySystem() { }

	void update(ComponentStore &store, const double delta) const override
	{
		for (auto &entity : getEntities())
		{
			std::shared_ptr<PositionComponent> pos = store.retrieve<PositionComponent>(entity);
			pos->frame.position.y += delta * 20;
		}
	}
};


#endif /* GRAVITYSYSTEM_H */
