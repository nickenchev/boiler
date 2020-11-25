#ifndef ANIMATIONSYSTEM_H
#define ANIMATIONSYSTEM_H

#include "core/system.h"

namespace Boiler
{

class AnimationSystem : public System
{
	double totalTime;
public:
	AnimationSystem() : System("Animation System")
	{
		totalTime = 0;
	}

	void update(ComponentStore &store, const double delta) override;
};

};

#endif /* ANIMATIONSYSTEM_H */
