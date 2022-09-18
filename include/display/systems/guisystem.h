#ifndef GUISYSTEM_H
#define GUISYSTEM_H

#include <memory>
#include <core/system.h>
#include <vector>

namespace Boiler
{

class GUISystem : public System
{
	AssetId materialId;
	std::vector<AssetId> primitives;

public:
	GUISystem(Renderer &renderer);
	~GUISystem();

	void mouseMove(uint32_t x, uint32_t y);
	void mouseButton(uint8_t button, bool isDown);
	void keyEvent(glm::int32_t key, bool isDown);

	void update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, ComponentStore &store) override;
};

}

#endif /* GUISYSTEM_H */
