#ifndef GRAVITYSYSTEM_H
#define GRAVITYSYSTEM_H

#include "core/system.h"
#include "core/components/positioncomponent.h"
#include "velocitycomponent.h"
#include "core/boiler.h"

class GravitySystem : public System
{
public:
	GravitySystem() { }

	void update(ComponentStore &store, const double delta) const override
	{
		Size screenSize = Boiler::getInstance().getScreenSize();

		for (auto &entity : getEntities())
		{
			std::shared_ptr<PositionComponent> pos = store.retrieve<PositionComponent>(entity);
			std::shared_ptr<VelocityComponent> vel = store.retrieve<VelocityComponent>(entity);

			vel->velocity.y += delta * 20;
			pos->frame.position.y += vel->velocity.y;

			if (pos->frame.position.y >= screenSize.getHeight() - pos->frame.size.getHeight())
			{
				vel->velocity.y = -(vel->velocity.y * 0.8f);
			}
		}
	}
};


#endif /* GRAVITYSYSTEM_H */
