#ifndef GUISYSTEM_H
#define GUISYSTEM_H

#include <memory>
#include <core/system.h>
#include <vector>
#include <assets/assetset.h>

namespace Boiler
{

class GUISystem : public System
{
	AssetSet assetSet;
	AssetId materialId;
	std::vector<AssetId> primitives;

public:
	GUISystem(Renderer &renderer);
	~GUISystem();

	void mouseMove(uint32_t x, uint32_t y);
	void mouseButton(uint8_t button, bool isDown);
	void mouseWheel(cgfloat x, cgfloat y);
	void keyEvent(glm::int32_t key, bool isDown);
	void keyMods(uint16_t keymods);
	void textInput(const std::string &text);

    void update(Renderer &renderer, AssetSet &assetSet, const FrameInfo &frameInfo, EntityComponentSystem &ecs) override;
};

}

#endif /* GUISYSTEM_H */
