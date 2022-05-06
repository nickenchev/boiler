#ifndef CAMERASYSTEM_H
#define CAMERASYSTEM_H

#include <core/system.h>

namespace Boiler
{
	class CameraSystem : public Boiler::System
	{
	public:
		CameraSystem();

		void update(FrameInfo frameInfo, ComponentStore &store) override;
	};
}

#endif /* CAMERASYSTEM_H */
