#ifndef GUISYSTEM_H
#define GUISYSTEM_H

#include <memory>
#include <core/system.h>

namespace Boiler
{

class GUISystem : public System
{
	AssetId materialId;
	AssetId primitiveId;

public:
	GUISystem(Renderer &renderer);
	~GUISystem();

	void update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, ComponentStore &store) override;
};

}

#endif /* GUISYSTEM_H */
