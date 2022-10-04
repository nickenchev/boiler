#ifndef TRANSFORMSYSTEM_H
#define TRANSFORMSYSTEM_H

#include "core/system.h"

namespace Boiler
{

struct FrameInfo;

class TransformSystem : public System
{
public:
	TransformSystem();

	void update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, ComponentStore &store) override;
};

}

#endif /* TRANSFORMSYSTEM_H */
